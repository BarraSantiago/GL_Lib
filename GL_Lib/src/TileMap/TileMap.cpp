#include "TileMap.h"
#include <fstream>
#include <fwd.hpp>
#include <sstream>
#include <ext/matrix_transform.hpp>

using namespace tinyxml2;

namespace gllib
{
    std::string ReadTextFile(const std::string& path)
    {
        std::ifstream f(path);
        if (!f.is_open()) throw std::runtime_error("ERROR. Could not open: " + path);
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }


    Tileset Tileset::LoadTSX(const std::string& tsxPath, int firstGidIn)
    {
        XMLDocument doc;
        if (doc.LoadFile(tsxPath.c_str()) != XML_SUCCESS)
        {
            throw std::runtime_error("ERROR. Could not read TSX: " + tsxPath);
        }

        XMLElement* tilesetElem = doc.FirstChildElement("tileset");
        if (!tilesetElem) throw std::runtime_error("ERROR. TSX sin <tileset> válido.");

        Tileset tileset;
        tileset.firstGid = firstGidIn;

        tilesetElem->QueryIntAttribute("tilewidth", &tileset.tileWidth);
        tilesetElem->QueryIntAttribute("tileheight", &tileset.tileHeight);
        tilesetElem->QueryIntAttribute("columns", &tileset.columns);
        tilesetElem->QueryIntAttribute("tilecount", &tileset.tileCount);

        XMLElement* imageElem = tilesetElem->FirstChildElement("image");
        if (!imageElem) throw std::runtime_error("TSX without <image>.");
        const char* src = imageElem->Attribute("source");
        tileset.imageSource = src ? src : "";

        imageElem->QueryIntAttribute("width", &tileset.imageWidth);
        imageElem->QueryIntAttribute("height", &tileset.imageHeight);

        // tile properties
        for (XMLElement* tileElem = tilesetElem->FirstChildElement("tile");
             tileElem;
             tileElem = tileElem->NextSiblingElement("tile"))
        {
            int id = -1;
            tileElem->QueryIntAttribute("id", &id);
            if (id < 0) continue;

            XMLElement* props = tileElem->FirstChildElement("properties");
            if (!props) continue;

            for (XMLElement* prop = props->FirstChildElement("property");
                 prop;
                 prop = prop->NextSiblingElement("property"))
            {
                const char* name = prop->Attribute("name");
                if (!name) continue;

                std::string n = name;
                if (n == "walkable")
                {
                    const char* val = prop->Attribute("value");
                    bool walk = true;
                    if (val)
                    {
                        std::string s = val;
                        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                        walk = !(s == "false" || s == "0");
                    }
                    tileset.walkableOverride[id] = walk;
                }
            }
        }

        return tileset;
    }
    
    uint32_t TileMap::ClearGidFlags(uint32_t gid)
    {
        return gid & 0x1FFFFFFF;
    }

    const Tileset* TileMap::findTilesetForGid(uint32_t gid) const
    {
        if (gid == 0) return nullptr;
        
        const Tileset* tileSet = nullptr;
        for (const Tileset& ts : tilesets)
        {
            if (gid >= static_cast<uint32_t>(ts.firstGid))
            {
                if (!tileSet || ts.firstGid > tileSet->firstGid)
                    tileSet = &ts;
            }
        }
        if (tileSet && tileSet->containsGid(gid)) return tileSet;
        return nullptr;
    }
    
    TileMap TileMap::LoadFromTiledXML(const std::string& tmxPath, unsigned int atlasTextureID)
    {
        XMLDocument doc;
        if (doc.LoadFile(tmxPath.c_str()) != XML_SUCCESS)
        {
            throw std::runtime_error("ERROR. Could not read TMX: " + tmxPath);
        }

        XMLElement* mapElem = doc.FirstChildElement("map");
        if (!mapElem) throw std::runtime_error("ERROR. TMX without valid <map> element.");

        TileMap map;

        mapElem->QueryIntAttribute("width", &map.width);
        mapElem->QueryIntAttribute("height", &map.height);
        mapElem->QueryIntAttribute("tilewidth", &map.tileWidth);
        mapElem->QueryIntAttribute("tileheight", &map.tileHeight);

        if (map.width <= 0 || map.height <= 0 || map.tileWidth <= 0 || map.tileHeight <= 0)
            throw std::runtime_error("Invalid map (dimensions).");

        // tilesets
        size_t lastSlash = tmxPath.find_last_of("/\\");
        std::string baseDir = (lastSlash == std::string::npos) ? "" : tmxPath.substr(0, lastSlash + 1);

        for (XMLElement* tsElem = mapElem->FirstChildElement("tileset");
             tsElem;
             tsElem = tsElem->NextSiblingElement("tileset"))
        {
            int firstgid = 1;
            tsElem->QueryIntAttribute("firstgid", &firstgid);

            const char* source = tsElem->Attribute("source");
            if (!source)
                throw std::runtime_error("Embedded tileset not supported. Use external TSX.");

            std::string tsxPath = baseDir + std::string(source);
            map.tilesets.push_back(Tileset::LoadTSX(tsxPath, firstgid));
        }

        std::sort(map.tilesets.begin(), map.tilesets.end(),
                  [](const Tileset& a, const Tileset& b) { return a.firstGid < b.firstGid; });

        // layers
        for (XMLElement* layerElem = mapElem->FirstChildElement("layer");
             layerElem;
             layerElem = layerElem->NextSiblingElement("layer"))
        {
            TileLayer layer;

            const char* name = layerElem->Attribute("name");
            layer.name = name ? name : "";

            layerElem->QueryIntAttribute("width", &layer.width);
            layerElem->QueryIntAttribute("height", &layer.height);

            int visible = 1;
            layerElem->QueryIntAttribute("visible", &visible);
            layer.visible = (visible != 0);

            if (layer.width == 0) layer.width = map.width;
            if (layer.height == 0) layer.height = map.height;

            XMLElement* dataElem = layerElem->FirstChildElement("data");
            if (!dataElem)
                throw std::runtime_error("Layer without <data> element.");

            const char* encoding = dataElem->Attribute("encoding");
            if (!encoding || std::string(encoding) != "csv")
                throw std::runtime_error("Only CSV encoding is supported. Set encoding='csv' in Tiled.");

            const char* csvText = dataElem->GetText();
            if (!csvText)
                throw std::runtime_error("Empty layer data.");

            // Parse CSV
            std::vector<uint32_t> gids;
            std::stringstream ss(csvText);
            std::string token;
            while (std::getline(ss, token, ','))
            {
                token.erase(0, token.find_first_not_of(" \t\n\r"));
                token.erase(token.find_last_not_of(" \t\n\r") + 1);

                if (!token.empty())
                    gids.push_back(static_cast<uint32_t>(std::stoul(token)));
            }

            if (static_cast<int>(gids.size()) != layer.width * layer.height)
                throw std::runtime_error("Data size doesn't match width*height.");

            layer.tiles.resize(layer.height);
            for (int row = 0; row < layer.height; ++row)
            {
                layer.tiles[row].resize(layer.width);
            }

            // Build tiles
            for (int row = 0; row < layer.height; ++row)
            {
                for (int col = 0; col < layer.width; ++col)
                {
                    int idx = row * layer.width + col;
                    uint32_t rawGid = gids[idx];
                    uint32_t gid = ClearGidFlags(rawGid);
            
                    if (gid != 0)
                    {
                        const Tileset* ts = map.findTilesetForGid(gid);
                        if (!ts)
                            throw std::runtime_error("Tileset not found for gid=" + std::to_string(gid));
            
                        Tile t(gid, ts->toLocalId(gid),
                               {static_cast<float>(col * map.tileWidth), static_cast<float>(row * map.tileHeight)},
                               ts->uvForLocalId(ts->toLocalId(gid)),
                               ts->isWalkable(ts->toLocalId(gid)),
                               map.tileWidth, map.tileHeight,
                               atlasTextureID);
            
                        layer.tiles[row][col] = t;
                    }
                    else
                    {
                        layer.tiles[row][col] = Tile();
                    }
                }
            }
            
            map.layers.push_back(std::move(layer));
        }

        return map;
    }
    
    void TileMap::draw() const
    {
        for (const TileLayer& layer : layers)
        {
            if (!layer.visible) continue;

            for (int x = 0; x < layer.height; ++x)
            {
                for (int y = 0; y < layer.width; ++y)
                {
                    const Tile& tile = layer.tiles[x][y];
                    if (!tile.empty())
                    {
                        const_cast<Tile&>(tile).draw();
                    }
                }
            }
        }
    }
    
    bool TileMap::checkCollisionAABB(float x, float y, float w, float h) const
    {
        Rect obj{x, y, w, h};

        // Rango de tiles tocados por el objeto
        int minCol = static_cast<int>(std::floor(x / tileWidth));
        int minRow = static_cast<int>(std::floor(y / tileHeight));
        int maxCol = static_cast<int>(std::floor((x + w - 0.001f) / tileWidth));
        int maxRow = static_cast<int>(std::floor((y + h - 0.001f) / tileHeight));

        minCol = std::max(0, minCol);
        minRow = std::max(0, minRow);
        maxCol = std::min(width - 1, maxCol);
        maxRow = std::min(height - 1, maxRow);

        if (minCol > maxCol || minRow > maxRow) return false;

        for (const TileLayer& layer : layers)
        {
            for (int row = minRow; row <= maxRow; ++row)
            {
                for (int col = minCol; col <= maxCol; ++col)
                {
                    const Tile& t = layer.tiles[row][col];
                    if (t.empty()) continue;
                    if (t.walkable) continue;

                    Rect tileRect{
                        t.getPosition().x,
                        t.getPosition().y,
                        static_cast<float>(tileWidth),
                        static_cast<float>(tileHeight)
                    };

                    if (Intersects(obj, tileRect))
                        return true;
                }
            }
        }

        return false;
    }
}

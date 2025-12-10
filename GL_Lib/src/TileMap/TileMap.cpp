#include "TileMap.h"
#include <fstream>
#include <fwd.hpp>
#include <sstream>
#include <ext/matrix_transform.hpp>

namespace gllib
{
    // --------------------
    // File utils
    // --------------------
    std::string ReadTextFile(const std::string& path)
    {
        std::ifstream f(path);
        if (!f.is_open()) throw std::runtime_error("ERROR. Could not open: " + path);
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    // --------------------
    // Tileset::LoadTSX
    // --------------------
    Tileset Tileset::LoadTSX(const std::string& tsxPath, int firstGidIn)
    {
        using namespace tinyxml2;

        XMLDocument doc;
        if (doc.LoadFile(tsxPath.c_str()) != XML_SUCCESS)
        {
            throw std::runtime_error("ERROR. Could not read TSX: " + tsxPath);
        }

        XMLElement* tilesetElem = doc.FirstChildElement("tileset");
        if (!tilesetElem) throw std::runtime_error("ERROR. TSX sin <tileset> válido.");

        Tileset ts;
        ts.firstGid = firstGidIn;

        tilesetElem->QueryIntAttribute("tilewidth", &ts.tileWidth);
        tilesetElem->QueryIntAttribute("tileheight", &ts.tileHeight);
        tilesetElem->QueryIntAttribute("columns", &ts.columns);
        tilesetElem->QueryIntAttribute("tilecount", &ts.tileCount);

        XMLElement* imageElem = tilesetElem->FirstChildElement("image");
        if (!imageElem) throw std::runtime_error("TSX without <image>.");
        const char* src = imageElem->Attribute("source");
        ts.imageSource = src ? src : "";

        imageElem->QueryIntAttribute("width", &ts.imageWidth);
        imageElem->QueryIntAttribute("height", &ts.imageHeight);

        // Leer propiedades por tile
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
                    // Tiled puede guardar bool como attribute "value"
                    // o en texto, aquí manejamos lo normal:
                    const char* val = prop->Attribute("value");
                    bool walk = true;
                    if (val)
                    {
                        std::string s = val;
                        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                        walk = !(s == "false" || s == "0");
                    }
                    ts.walkableOverride[id] = walk;
                }
            }
        }

        return ts;
    }

    // --------------------
    // Tilemap helpers
    // --------------------
    // Flags típicos de Tiled en GID (flip horizontal/vertical/diagonal).
    uint32_t TileMap::ClearGidFlags(uint32_t gid)
    {
        // 0xE0000000 son los 3 bits altos usados para flips.
        return gid & 0x1FFFFFFF;
    }

    const Tileset* TileMap::findTilesetForGid(uint32_t gid) const
    {
        if (gid == 0) return nullptr;

        // Tiled define firstgid por tileset, ordenados por firstgid.
        // El tileset correcto es el de mayor firstgid <= gid.
        const Tileset* best = nullptr;
        for (const Tileset& ts : tilesets)
        {
            if (gid >= static_cast<uint32_t>(ts.firstGid))
            {
                if (!best || ts.firstGid > best->firstGid)
                    best = &ts;
            }
        }
        if (best && best->containsGid(gid)) return best;
        return nullptr;
    }

    // --------------------
    // TileMap::LoadFromTiledXML
    // --------------------
    TileMap TileMap::LoadFromTiledXML(const std::string& tmxPath)
    {
        using namespace tinyxml2;
    
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
    
        // ---- tilesets ----
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
    
        // ---- layers ----
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
                // Trim whitespace
                token.erase(0, token.find_first_not_of(" \t\n\r"));
                token.erase(token.find_last_not_of(" \t\n\r") + 1);
                
                if (!token.empty())
                    gids.push_back(static_cast<uint32_t>(std::stoul(token)));
            }
    
            if (static_cast<int>(gids.size()) != layer.width * layer.height)
                throw std::runtime_error("Data size doesn't match width*height.");
    
            layer.tiles.resize(layer.height);
            for (int r = 0; r < layer.height; ++r)
            {
                layer.tiles[r].resize(layer.width);
            }
    
            // Build tiles
            for (int r = 0; r < layer.height; ++r)
            {
                for (int c = 0; c < layer.width; ++c)
                {
                    int idx = r * layer.width + c;
                    uint32_t rawGid = gids[idx];
                    uint32_t gid = ClearGidFlags(rawGid);
    
                    Tile t;
                    t.gid = gid;
                    t.worldPos = {static_cast<float>(c * map.tileWidth), static_cast<float>(r * map.tileHeight)};
    
                    if (gid != 0)
                    {
                        const Tileset* ts = map.findTilesetForGid(gid);
                        if (!ts)
                            throw std::runtime_error("Tileset not found for gid=" + std::to_string(gid));
    
                        t.localId = ts->toLocalId(gid);
                        t.uv = ts->uvForLocalId(t.localId);
                        t.walkable = ts->isWalkable(t.localId);
                    }
    
                    layer.tiles[r][c] = t;
                }
            }
    
            map.layers.push_back(std::move(layer));
        }
    
        return map;
    }

    // --------------------
    // TileMap::draw
    // --------------------
    void TileMap::draw(unsigned int atlasTextureID) const
    {
        float quadVertexData[] = {
             1.0f, 1.0f, 0.0f, /* xyz */ 1.0f, 1.0f, 1.0f, 1.0f, /* rgba */ 1.0f, 0.0f, /* uv */
             1.0f, 0.0f, 0.0f, /* xyz */ 1.0f, 1.0f, 1.0f, 1.0f, /* rgba */ 1.0f, 1.0f, /* uv */
             0.0f, 0.0f, 0.0f, /* xyz */ 1.0f, 1.0f, 1.0f, 1.0f, /* rgba */ 0.0f, 1.0f, /* uv */
             0.0f, 1.0f, 0.0f, /* xyz */ 1.0f, 1.0f, 1.0f, 1.0f, /* rgba */ 0.0f, 0.0f, /* uv */
        };
    
        const int quadIndex[] = {
            0, 1, 3,
            1, 2, 3
        };
    
        static RenderData tileRenderData = {0, 0, 0};
        if (tileRenderData.VAO == 0) {
            tileRenderData = Renderer::createRenderData(
                quadVertexData,
                sizeof(quadVertexData) / sizeof(float),
                quadIndex,
                sizeof(quadIndex) / sizeof(int)
            );
        }
    
        for (const TileLayer& layer : layers)
        {
            if (!layer.visible) continue;
    
            for (int r = 0; r < layer.height; ++r)
            {
                for (int c = 0; c < layer.width; ++c)
                {
                    const Tile& t = layer.tiles[r][c];
                    if (t.empty()) continue;
    
                    // Update UV coordinates
                    quadVertexData[7] = t.uv.u1; quadVertexData[8] = t.uv.v0;
                    quadVertexData[16] = t.uv.u1; quadVertexData[17] = t.uv.v1;
                    quadVertexData[25] = t.uv.u0; quadVertexData[26] = t.uv.v1;
                    quadVertexData[34] = t.uv.u0; quadVertexData[35] = t.uv.v0;
    
                    glBindBuffer(GL_ARRAY_BUFFER, tileRenderData.VBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertexData), quadVertexData);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
                    // Set transformation matrix
                    glm::mat4 trs = glm::translate(glm::mat4(1.0f),
                        glm::vec3(t.worldPos.x, t.worldPos.y, 0.0f));
                    trs = glm::scale(trs, glm::vec3(static_cast<float>(tileWidth), static_cast<float>(tileHeight), 1.0f));
    
                    Renderer::setModelMatrix(trs);
                    
                    // Bind texture BEFORE each draw call
                    Renderer::bindTexture(atlasTextureID);
                    Renderer::drawElements(tileRenderData, 6);
                }
            }
        }
    }

    // --------------------
    // TileMap::checkCollisionAABB
    // --------------------
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
            for (int r = minRow; r <= maxRow; ++r)
            {
                for (int c = minCol; c <= maxCol; ++c)
                {
                    const Tile& t = layer.tiles[r][c];
                    if (t.empty()) continue;
                    if (t.walkable) continue;

                    Rect tileRect{
                        t.worldPos.x,
                        t.worldPos.y,
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

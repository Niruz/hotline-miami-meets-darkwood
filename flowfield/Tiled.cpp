

struct XMLTile
{
	std::string myID;
	std::map<std::string, std::string> myProperties;
};

XMLTile XMLTILE(const std::string& id)
{
	XMLTile tile;
	tile.myID = id;
	return tile;
}

static void 
AddProperty(XMLTile* xmlTile, const std::string& name, const std::string& value)
{
	xmlTile->myProperties[name] = value;
}

static std::string 
GetProperty(XMLTile* xmlTile, const std::string& name)
{
	return xmlTile->myProperties[name];
}

static bool 
HasProperty(XMLTile* xmlTile, const std::string& name)
{
	if (xmlTile->myProperties.find(name) == xmlTile->myProperties.end())
	{
		// not found
		return false;
	}
	else
	{
		// found
		return true;
	}
}

static bool 
GetPropertyBooleanValue(XMLTile* xmlTile, const std::string& name)
{
	if (xmlTile->myProperties[name] == "true")
		return true;
	else
		false;
}

struct XMLLayer
{
	std::string myName;
	std::string myWidth;
	std::string myHeight;

	std::vector<std::string> myData;

	//Tile.. object layer etc
	std::string myType;
};

XMLLayer XMLLAYER(const std::string& identifier, const std::string& type, const std::string& width, const std::string& height)
{
	XMLLayer layer;
	layer.myName = identifier;
	layer.myType = type;
	layer.myWidth = width;
	layer.myHeight = height;
	return layer;
}

static void
SetData(XMLLayer* layer, std::vector<std::string> data) 
{
	layer->myData = data;
}

struct XMLMap
{
	std::map<std::string, XMLLayer> myLayers;
};

static void 
AddLayer(XMLMap* map, const std::string& identifier, const XMLLayer& layer)
{
	map->myLayers[identifier] = layer;
}

static XMLLayer* 
GetLayer(XMLMap* map,const std::string& identifier)
{
	return &map->myLayers[identifier];
}

struct TiledMapManager
{
	//These are the nodes describing a tiles properties
	std::map<std::string, std::map<std::string, XMLTile>> xmlTilePropertyNodes;
	std::map<std::string, XMLMap> myXMLMaps;
};

TiledMapManager tiledMapManager;

static void 
LoadTileset(TiledMapManager* tiledMapManager, const std::string& name, const std::string& identifier)
{
	rapidxml::file<> tileFile(name.c_str()); // Default template is char
	rapidxml::xml_document<> tilesetDoc;
	tilesetDoc.parse<0>(tileFile.data());

	std::cout << tilesetDoc.first_node()->name() << std::endl;

	rapidxml::xml_node<> *tileSetRoot = tilesetDoc.first_node();

	std::map<std::string, XMLTile> xmlTiles;

	//Load the tilesets with their 
	for (rapidxml::xml_node<> *pNode = tileSetRoot->first_node("tile"); pNode; pNode = pNode->next_sibling())
	{
		rapidxml::xml_node<>* propertiesNode = pNode->first_node("properties");

		std::string currentID = std::to_string(std::stoi(pNode->first_attribute("id")->value()) + 1);

		xmlTiles[currentID] = XMLTILE(currentID);

		for (rapidxml::xml_node<> *propertyNode = propertiesNode->first_node("property"); propertyNode; propertyNode = propertyNode->next_sibling())
		{
			AddProperty(&xmlTiles[currentID], propertyNode->first_attribute("name")->value(), propertyNode->first_attribute("value")->value());
		}
	}

	tiledMapManager->xmlTilePropertyNodes[identifier] = xmlTiles;
}

static std::map<std::string, XMLTile>& 
GetTileset(TiledMapManager* mapManager, const std::string& identifier)
{
	return mapManager->xmlTilePropertyNodes[identifier];
}

static void 
LoadMap(TiledMapManager* mapManager, const std::string& file, const std::string& identifier)
{
	rapidxml::file<> mapFile(file.c_str());
	rapidxml::xml_document<> mapDoc;
	mapDoc.parse<0>(mapFile.data());

	std::cout << mapDoc.first_node()->name() << std::endl;

	rapidxml::xml_node<> *mapRoot = mapDoc.first_node();

	XMLMap xmlMap;

	//Load the tilesets with their 
	for (rapidxml::xml_node<> *pNode = mapRoot->first_node("layer"); pNode; pNode = pNode->next_sibling())
	{
		std::string name = pNode->first_attribute("name")->value();
		std::string width = pNode->first_attribute("width")->value();
		std::string height = pNode->first_attribute("height")->value();

		std::string tileData = pNode->first_node("data")->value();
		tileData.erase(std::remove(tileData.begin(), tileData.end(), '\r'), tileData.end());
		tileData.erase(std::remove(tileData.begin(), tileData.end(), '\n'), tileData.end());

		XMLLayer xmlLayer = XMLLAYER(identifier, name, width, height);
		SetData(&xmlLayer, EngineUtilities::Split(tileData, ','));

		AddLayer(&xmlMap, name, xmlLayer);
	}

	mapManager->myXMLMaps[identifier] = xmlMap;
}

static XMLMap*
GetMap(TiledMapManager* mapManager, const std::string& identifier)
{
	return &mapManager->myXMLMaps[identifier];
}

static void
OnInitialize(TiledMapManager* tiledMapManager)
{
	//Tilesets
	LoadTileset(tiledMapManager, "Levels/rave1.tsx", "TestTileset");

	//Maps
	LoadMap(tiledMapManager, "Levels/untitled.tmx", "TestMap");
}

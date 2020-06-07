/*
 * ResourceManager.cpp
 *
 *  Created on: 7 Jun 2020
 *      Author: xxMrPHDxx
 */

#include "ResourceManager.h"

std::map<std::string, Textures> ResourceManager::textures;

std::string ResourceManager::load_textfile(std::string path){
	std::ifstream ifs(path);
	std::stringstream ss;
	std::string line;
	while(ifs >> line) ss << line << "\n";
	line = ss.str();
	return line.c_str();
}

void ResourceManager::load_texture(std::string tex_path){
	std::ifstream ifs(tex_path);
	if(!ifs.is_open()){
		printf("File '%s' can't be opened!\n", tex_path.c_str());
		exit(-1);
	}
	std::string path, last_name, name;
	bool first = true;
	std::vector<sf::Texture*> list;
	for(;!ifs.eof() && std::getline(ifs, name, '\\') && std::getline(ifs, path);){
		// Get the full path
		path = "Textures\\"+name+'\\'+path;

		// If it's a new name, insert list into the map and empty out the list
		if(last_name != name && !first){
			textures.insert({last_name, list});
			list.clear();
		}
		first = false;

		// Create the textures and put it in the list
		sf::Texture* tex = new sf::Texture();
		tex->loadFromFile(path);
		list.push_back(tex);
		last_name = name;
	}
}

void ResourceManager::clean(){
	for(std::pair<std::string, Textures> pair : textures){
		for(sf::Texture* ptr : pair.second){
			delete ptr;
		}
	}
}

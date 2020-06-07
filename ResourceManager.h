/*
 * ResourceManager.h
 *
 *  Created on: 7 Jun 2020
 *      Author: xxMrPHDxx
 */

#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include "pch.h"

typedef std::vector<sf::Texture*> Textures;

class ResourceManager {
public:
	static std::map<std::string, Textures> textures;
	static std::string load_textfile(std::string);
	static void load_texture(std::string);
	static void clean();
};

#endif /* RESOURCEMANAGER_H_ */

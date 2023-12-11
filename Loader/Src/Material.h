#pragma once

#include "IMaterial.h"

#include "rapidjson/include/rapidjson/document.h"

void _LoadMaterial(const rapidjson::Value& JsonObject, MATERIAL_SOURCE& Subset);
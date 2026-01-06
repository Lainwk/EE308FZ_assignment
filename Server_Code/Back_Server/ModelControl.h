#pragma once
#include "BaseModel.h"
#include <memory>
#include <string>

// Simple singleton wrapper to reuse a shared BaseModel instance.
class ModelControl {
public:
    ~ModelControl();
    static ModelControl* getInstance();
    BaseModel* getModel();

private:
    ModelControl();
    static ModelControl* instance;
    std::unique_ptr<BaseModel> model;
};


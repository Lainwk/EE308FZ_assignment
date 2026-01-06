#include "ModelControl.h"

ModelControl* ModelControl::instance = nullptr;

ModelControl::ModelControl() {
    model = std::make_unique<BaseModel>();
}

ModelControl::~ModelControl() = default;

ModelControl* ModelControl::getInstance() {
    if (instance == nullptr) {
        instance = new ModelControl();
    }
    return instance;
}

BaseModel* ModelControl::getModel() {
    return model.get();
}


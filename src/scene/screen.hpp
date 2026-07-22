#pragma once

class Screen {
public:
    virtual ~Screen() = default;

    virtual void update() {}
    virtual void render() {}
};
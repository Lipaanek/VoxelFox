#pragma once

class Screen {
    public:
        virtual ~Screen() = default;

        virtual void OnEnter() {}
        virtual void OnExit() {}

        virtual void Update() = 0;
        virtual void Render() = 0;
};
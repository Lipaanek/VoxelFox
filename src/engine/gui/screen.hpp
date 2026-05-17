#pragma

class Screen {
    public:
        virtual ~Screen() = default;

        virtual void Update() = 0;
        virtual void Render() = 0;
};
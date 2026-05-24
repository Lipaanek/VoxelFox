#ifndef SYSTEM_HPP
#define SYSTEM_HPP

class System {
    public:
        virtual ~System() = default;
        virtual void Update(float dt) = 0;
};

#endif
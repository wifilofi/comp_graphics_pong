#pragma once

#include "../Render/Pipeline.h"
#include "../Input/PC.h"
#include "../Physics/Collide.h"
#include "../Physics/Move.h"
#include "../Render/Window.h"
#include "../Update/Fixed.h"

class Game final
{
public:
    static Game* Instance();

    void Construct(const char16* pName, float fixedDelta) const;
    void Run();
    void Destroy() const;

    Engine::Render::Window* GetWindow() const { return window_; }
    Engine::Update::Fixed* GetFixedUpdate() const { return fixedUpdate_; }
    Engine::Render::Pipeline* GetRenderPipeline() const { return pipeline_; }
    Engine::Input::Device* GetInputDevice() const { return inputDevice_; }
    Engine::Physics::Collide* GetPhysicsCollide() const { return physicsCollide_; }
    Engine::Physics::Move* GetPhysicsMove() const { return physicsMove_; }

private:
    void Input();
    void Update();

    Engine::Render::Window* window_ = new Engine::Render::Window();
    Engine::Update::Fixed* fixedUpdate_ = new Engine::Update::Fixed();
    Engine::Render::Pipeline* pipeline_ = new Engine::Render::Pipeline();
    Engine::Input::Device* inputDevice_ = new Engine::Input::PC();
    Engine::Physics::Collide* physicsCollide_ = new Engine::Physics::Collide();
    Engine::Physics::Move* physicsMove_ = new Engine::Physics::Move();

    bool isFinished_ = false;
};

#include "Gameplay/Components/PlayerMovementBehavior.h"
#include <GLFW/glfw3.h>
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"
#include "Application/Application.h"
#include "Application/Layers/PostProcessingLayer.h"
#include "Gameplay/InputEngine.h"

#include "Gameplay/Material.h"
#include "Gameplay/MeshResource.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Graphics/Textures/Texture2D.h"
#include "Utils/GlmDefines.h"
#include "Gameplay/Components/SeekBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"



//create movetowards function
//glm::quat MoveTowards(glm::quat current, glm::quat target, float maxDistanceDelta)
//{
//	glm::quat a = glm::quat(glm::vec3(target.x - current.x, target.y - current.y, target.z - current.z));
//	//a.x = target.x - current.x;
//	//a.y = target.y - current.y;
//	//a.z = target.z - current.z;
//	glm::vec3 magnitude2 = glm::vec3(a.x, a.y, a.z);
//	float magnitude = magnitude2.length();
//	if (magnitude <= maxDistanceDelta || magnitude == 0.0f)
//	{
//		return target;
//	}
//	return current + a / magnitude * maxDistanceDelta;
//}
//float MoveTowards2(float current, float target, float maxDelta)
//{
//	if (glm::abs(target - current) <= maxDelta)
//	{
//		return target;
//	}
//	return current + glm::sign(target - current) * maxDelta;
//}
void PlayerMovementBehavior::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	if (body->GetGameObject()->Name == "Enemy") //this trigger has entered to the player
	{
		LOG_WARN("YOU LOST! You can't touch enemies!");
		Application::Get().Quit();
	}

	if (body->GetGameObject()->Name == "ThingProjectile" && InputEngine::IsKeyDown(GLFW_KEY_SPACE))
	{
		//isReturned = true;
		//maybe put in key input
		//destroy thing it comes in contact with
		Application& app = Application::Get();
		Gameplay::GameObject::Sptr context = body->GetGameObject()->SelfRef();

		context->Get<SeekBehaviour>()->seekTo(app.CurrentScene()->FindObjectByName("Enemy"));
	}

	else if (body->GetGameObject()->Name == "ThingProjectile")
	{
		LOG_WARN("YOU LOST! don't get shot 5head");
		Application::Get().Quit();

		Gameplay::GameObject::Sptr context = body->GetGameObject()->SelfRef();
		Application::Get().CurrentScene()->RemoveGameObject(context);
	}

	

}

void PlayerMovementBehavior::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
	
}
void PlayerMovementBehavior::Awake()
{
	_body = GetComponent<Gameplay::Physics::RigidBody>(); //Gets the Physics Body that it's attached to
	if (_body == nullptr) {
		IsEnabled = false;
	}

	/*_body->SetLinearDamping(0.9f);
	_body->SetAngularDamping(1.f);*/
}

void PlayerMovementBehavior::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat, "Impulse", &_impulse, 1.0f);
}

nlohmann::json PlayerMovementBehavior::ToJson() const {
	return {
		{ "impulse", _impulse }, {"spill_state", in_spill}, {"moving", is_moving}
	};
}

PlayerMovementBehavior::PlayerMovementBehavior() :
	IComponent(),
	_impulse(0.0f) //Movement Value
{ }

PlayerMovementBehavior::~PlayerMovementBehavior() = default;

PlayerMovementBehavior::Sptr PlayerMovementBehavior::FromJson(const nlohmann::json & blob) {
	PlayerMovementBehavior::Sptr result = std::make_shared<PlayerMovementBehavior>();
	result->_impulse = blob["impulse"];
	result->in_spill = blob["spill_state"];
	result->is_moving = blob["moving"];
	return result;
}

void PlayerMovementBehavior::SetSpill(bool state)
{
	in_spill = state;
}

void PlayerMovementBehavior::Update(float deltaTime) {

	Application& app = Application::Get();
	//not moving
	

	//IF SPACE PRESSED = MOVE
	is_moving = false;
	if (glfwGetKey(app.GetWindow(), GLFW_KEY_W) || glfwGetKey(app.GetWindow(), GLFW_KEY_UP)) {
		if (_body->GetLinearVelocity().y >= -5.0f) {
			GetGameObject()->SetRotation(glm::vec3(90, 0, 0));
			
			_body->ApplyImpulse(glm::vec3(0.0f, -_impulse, 0.0f));
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			//body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(0.0f, -_impulse, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_S) || glfwGetKey(app.GetWindow(), GLFW_KEY_DOWN)) {
		if (_body->GetLinearVelocity().y <= 5.0f) {
			_body->ApplyImpulse(glm::vec3(0.0f, _impulse, 0.0f));
			GetGameObject()->SetRotation(glm::vec3(90, 0, -180));
		
			
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 180.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(0.0f, _impulse, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_A) || glfwGetKey(app.GetWindow(), GLFW_KEY_LEFT)) {
		if (_body->GetLinearVelocity().x <= 5.0f) {
			_body->ApplyImpulse(glm::vec3(_impulse, 0.0f, 0.0f));
			GetGameObject()->SetRotation(glm::vec3(90, 0, -270));
			

			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, 90.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(_impulse, 0.0f, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_D) || glfwGetKey(app.GetWindow(), GLFW_KEY_RIGHT)) {
		if (_body->GetLinearVelocity().x >= -5.0f) {
			_body->ApplyImpulse(glm::vec3(-_impulse, 0.0f, 0.0f));
			GetGameObject()->SetRotation(glm::vec3(90,0,-90));
			

			
			//GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, -90.0f));
			//_body->GetGameObject()->SetPostion(_body->GetGameObject()->GetPosition() + glm::vec3(-_impulse, 0.0f, 0.0f));
			is_moving = true;
			_impulse += acceleration;
			if (_impulse > max_speed)
			{
				_impulse = max_speed;
			}
		}
	}
	
	if (InputEngine::IsKeyDown(GLFW_KEY_SPACE)) {
		
	
		
	}
	

	if (!is_moving)
	{
		if (_impulse < 0)
		{
			_impulse += acceleration;
			if (_impulse >= 0)
			{
				_impulse = 0.0f;
			}
		}
		else
		{
			_impulse -= acceleration;
			if (_impulse <= 0)
			{
				_impulse = 0.0f;
			}
		}

	}

	//particle stuff
	if (is_moving)
	{
		GetGameObject()->GetChildren()[0]->Get<ParticleSystem>()->IsEnabled = true;
	}
	else
	{
		//GetGameObject()->GetChildren()[0]->Get<ParticleSystem>()->
		GetGameObject()->GetChildren()[0]->Get<ParticleSystem>()->IsEnabled = false;
	}
}


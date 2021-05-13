#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Options destinées au mouvement de la caméra
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Valeurs par défaut de la caméra
const float YAW					= -90.0f;		// Angle de rotation autour de l'axe Y
const float PITCH				=  0.0f;		// Angle de rotation autour de l'axe X
const float SPEED				=  0.005f;		// Vitesse de déplacement
const float MOUSE_SENSITIVITY	=  0.1f;		// Sensibilité de la caméra
const float FOV					=  45.0f;		// Champ de vision
const float ZOOM_SENSITIVITY	=  2.0f;		// Sensibilité de la molette de zoom

class Camera
{
public:
	// Attributs de la caméra
	glm::vec3 Position;						// Position de la caméra
	glm::vec3 Front;						// Axe z de la caméra (direction dans laquelle elle regarde)
	glm::vec3 Up;							// Axe y de la caméra
	glm::vec3 Right;						// Axe x de la caméra
	glm::vec3 WorldUp;						// Axe y du monde
	// Angles de Euler
	float Yaw;
	float Pitch;
	// Options de la caméra
	float MovementSpeed;
	float MouseSensitivity;
	float Fov;
	float ZoomSensitivity;

	// Constructeur
	Camera (glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(MOUSE_SENSITIVITY), Fov(FOV), ZoomSensitivity(ZOOM_SENSITIVITY)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Retourne la ViewMatrix calculée avec les angles d'Euler et la matrice LookAt
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Effectue le déplacement de la caméra grâce aux touches du clavier
	void MotionKeyboard (Camera_Movement direction, float deltaTime)
	{
		// Calcul de la vélocité à partir du taux de rafraîchissement (deltaTime) et la vitesse de déplacement
		float velocity = MovementSpeed * deltaTime;		

		// Mise à jour de la position en fonction de la direction entrée en paramètres
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
		if (direction == UP)
			Position += WorldUp * velocity;
		if (direction == DOWN)
			Position -= WorldUp * velocity;
	}

	// Effectue le changement de POV de la caméra grâce à la souris
	void MotionMouse(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		// Prise en compte de la sensibilité de la souris lors du déplacement
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		// Mise à jour des angles d'Euler
		Yaw += xoffset;
		Pitch -= yoffset;

		// Evite un flip de caméra au cas où on la tournerait trop vers le haut ou vers le bas
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Recalcul des axes de la caméra grâce aux nouveaux angles
		updateCameraVectors();
	}

	// Effectue le zoom de la caméra grâce à la molette
	void MotionScroll(float yoffset)
	{
		// Prise en compte de la sensibilité du Zoom
		yoffset *= ZoomSensitivity;

		// Mise à jour du champ de vision
		Fov -= (float)yoffset;

		// Pose une limite de zoom et de dé-zoom
		if (Fov < 1.0f)
			Fov = 1.0f;
		if (Fov > 45.0f)
			Fov = 45.0f;
	}

	void Reset(glm::vec3 newPosition)
	{
		Position = newPosition;
		Yaw = YAW;
		Pitch = -25.0f;
		updateCameraVectors();
	}

private:
	// Calcule l'axe Z de la caméra à partir des angles d'Euler
	void updateCameraVectors()
	{
		// Calcul du nouvel axe Z de la caméra
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		// Recalcul des axe X et Y de la caméra
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalisation des vecteurs car leur longueur tend vers 0 à mesure qu'on lève ou baisse la tête par exemple
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Options destin�es au mouvement de la cam�ra
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Valeurs par d�faut de la cam�ra
const float YAW					= -90.0f;		// Angle de rotation autour de l'axe Y
const float PITCH				=  0.0f;		// Angle de rotation autour de l'axe X
const float SPEED				=  0.005f;		// Vitesse de d�placement
const float MOUSE_SENSITIVITY	=  0.1f;		// Sensibilit� de la cam�ra
const float FOV					=  45.0f;		// Champ de vision
const float ZOOM_SENSITIVITY	=  2.0f;		// Sensibilit� de la molette de zoom

class Camera
{
public:
	// Attributs de la cam�ra
	glm::vec3 Position;						// Position de la cam�ra
	glm::vec3 Front;						// Axe z de la cam�ra (direction dans laquelle elle regarde)
	glm::vec3 Up;							// Axe y de la cam�ra
	glm::vec3 Right;						// Axe x de la cam�ra
	glm::vec3 WorldUp;						// Axe y du monde
	// Angles de Euler
	float Yaw;
	float Pitch;
	// Options de la cam�ra
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

	// Retourne la ViewMatrix calcul�e avec les angles d'Euler et la matrice LookAt
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Effectue le d�placement de la cam�ra gr�ce aux touches du clavier
	void MotionKeyboard (Camera_Movement direction, float deltaTime)
	{
		// Calcul de la v�locit� � partir du taux de rafra�chissement (deltaTime) et la vitesse de d�placement
		float velocity = MovementSpeed * deltaTime;		

		// Mise � jour de la position en fonction de la direction entr�e en param�tres
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

	// Effectue le changement de POV de la cam�ra gr�ce � la souris
	void MotionMouse(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		// Prise en compte de la sensibilit� de la souris lors du d�placement
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		// Mise � jour des angles d'Euler
		Yaw += xoffset;
		Pitch -= yoffset;

		// Evite un flip de cam�ra au cas o� on la tournerait trop vers le haut ou vers le bas
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Recalcul des axes de la cam�ra gr�ce aux nouveaux angles
		updateCameraVectors();
	}

	// Effectue le zoom de la cam�ra gr�ce � la molette
	void MotionScroll(float yoffset)
	{
		// Prise en compte de la sensibilit� du Zoom
		yoffset *= ZoomSensitivity;

		// Mise � jour du champ de vision
		Fov -= (float)yoffset;

		// Pose une limite de zoom et de d�-zoom
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
	// Calcule l'axe Z de la cam�ra � partir des angles d'Euler
	void updateCameraVectors()
	{
		// Calcul du nouvel axe Z de la cam�ra
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		// Recalcul des axe X et Y de la cam�ra
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalisation des vecteurs car leur longueur tend vers 0 � mesure qu'on l�ve ou baisse la t�te par exemple
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif
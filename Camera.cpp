#include "IndProj.h"

void Camera::WorldToScreen(Vector &position)
{
	position -= this->Position;
	position *= Zoom;
}
float Camera::WorldToScreenX(float posX)
{
	return (posX - Position.X) * Zoom;
}
float Camera::WorldToScreenY(float posX)
{
	return (posX - Position.Y) * Zoom;
}

void Camera::ScreenToWorld(Module module, Vector &position)
{
	position += this->Position * Zoom;
	float cellSize = module.GetCellSize();
	position.X = position.X / cellSize;
	position.Y = position.Y / cellSize;
	position /= Zoom;
	position.X = int(position.X);
	position.Y = int(position.Y);
}
#include "IndProj.h"
#include <iostream>


void Cell::CopyTempsFrom(Cell other)
{
    this->burning = other.burning;
    this->fuelMass = other.fuelMass;
    this->temp = other.temp;
    this->waterTemp = other.waterTemp;
    this->waterMass = other.waterMass;
}
void Cell::CopyConstantsFrom(Cell other)
{
    this->isWater = other.isWater;
    this->capacity = other.capacity;
    this->burnSpeed = other.burnSpeed;
    this->combustionHeat = other.combustionHeat;
    this->fireTemp = other.fireTemp;
    this->height = other.height;
    this->colorR = other.colorR;
    this->colorG = other.colorG;
    this->colorB = other.colorB;

   // y = 
}

void Cell::SetHumidity(float percent)
{
    // ������ ����� �������� ���� �������� ��������� ����������-����������
    waterMass = 100000.0 * 0.018 * 0.5 / (8.31 * (273.0 + temp)) * (percent / 100.0);
}
float Cell::GetHumidity()
{
    return waterMass * 8.31 * (273.0 + temp) / (100000.0 * 0.5 * 0.018);
}

int RandInt(int min, int max)
{
    return (rand() % (max - min) + min + 1);
}
void Cell::FillParams()
{
    isWater = false;
    burning = false;
    fuelMass = (float)RandInt(300, 500) / 1000.0;
    capacity = RandInt(350, 400);
    fireTemp = (float)RandInt(230, 300);
    temp = waterTemp = 20;
    SetHumidity(RandInt(30, 40));
    burnSpeed = 0.025;
    combustionHeat = (float)RandInt(1200000, 1700000);
    height = 100;
    colorR = 10; colorG = 180; colorB = 60;
}

void Cell::Update(float heat, float deltaTime)
{
    if (fuelMass <= 0 || isWater) return;

    if (burning)
    {
        fuelMass -= burnSpeed * deltaTime; // ������� ������� 
        if (fuelMass <= 0.0) // ���� �������� �� ��������
        {
            fuelMass = 0.0;
            burning = false; // ������ ������
        }
        return;
    }

    if (waterMass > 0) // �������� �� ������� �������� ����
    {
        // ���� ����������� ���� ������ 100 �������� => ������ ����
        if (waterTemp < VaporizationTemp) 
        {
            // ������ ����������� ������� �� �������������
            float deltaTemp = heat / (WaterCapacity * waterMass);

            if (waterTemp + deltaTemp > VaporizationTemp)
            {
                heat -= WaterCapacity * waterMass * (VaporizationTemp - waterTemp);
                waterTemp = VaporizationTemp;
            }
            else
            {
                heat = 0;
                waterTemp += deltaTemp;
            }
        }
        else // ��������� ����
        {
            // ������ ����� ����������� ���� �� ������� �� �������������
            float waterVaporized = heat / VaporizationHeat;
            if (waterMass - waterVaporized <= 0)
            {
                waterMass = 0;
                heat -= waterMass * VaporizationHeat;
            }
            else
            {
                waterMass -= waterVaporized * deltaTime;
                heat = 0;
            }
        }
    }

    temp += heat / (capacity * fuelMass) * deltaTime;
    // �������� ������� �� ������, ���������� ����� ���� �� ������

    if (temp >= fireTemp && !burning) // ���� ����������� ����
    {                        // ���� ����������� �������������
        burning = true;
    }
}


COLORREF Cell::GetColor()
{
    if (this->fuelMass <= 0) return RGB(40, 25, 15);
    if (isWater) return RGB(40, 60, 230);

    return RGB(
        min(255, colorR + (255-colorR) * (temp / fireTemp)),
        min(255, colorG * min((fuelMass / 1), 1)),
        min(255, colorB * GetHumidity()));
}
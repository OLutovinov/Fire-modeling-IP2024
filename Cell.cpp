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
    this->capacity = other.capacity;
    this->burnSpeed = other.burnSpeed;
    this->combustionHeat = other.combustionHeat;
    this->fireTemp = other.fireTemp;
    this->height = other.height;
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
    burning = false;
    fuelMass = (float)RandInt(200, 300) / 1000.0;
    capacity = RandInt(300, 400);
    fireTemp = (float)RandInt(230, 300);
    temp = waterTemp = 20;
    SetHumidity(RandInt(10, 20));
    burnSpeed = 0.025 * 1.0;
    combustionHeat = (float)RandInt(800000, 1200000);
    height = 100;
}

void Cell::Update(float heat)
{
    if (fuelMass <= 0) return;

    if (burning)
    {
        fuelMass -= burnSpeed; // ������� ������� 
        if (fuelMass <= 0.0) // ���� �������� �� ��������
        {
            fuelMass = 0.0;
            burning = false; // ������ ������
        }
        return;
    }

    if (waterMass > 0.0) // �������� �� ������� �������� ����
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
        if (waterTemp >= VaporizationTemp)  // ��������� ����
        {
            // ������ ����� ����������� ���� �� ������� �� �������������
            float waterVaporized = heat / VaporizationHeat;
            if (waterMass - waterVaporized <= 0.0)
            {
                heat -= waterMass * VaporizationHeat;
                waterMass = 0;
            }
            else
            {
                waterMass -= waterVaporized;
                heat = 0;
            }
        }
    }

    temp += heat / (capacity * fuelMass);
    // �������� ������� �� ������, ���������� ����� ���� �� ������

    if (temp >= fireTemp && !burning) // ���� ����������� ����
    {                        // ���� ����������� �������������
        burning = true;
    }
}


COLORREF Cell::GetColor()
{
    if (this->fuelMass <= 0 && GetHumidity() < 0.25) return RGB(40, 25, 15);
    
    return RGB(
        min(255, 255 * (temp / fireTemp)),
        min(255, 255 * (fuelMass / 0.5)),
        min(255, 255 * GetHumidity()));
}
#define ACCEL_THRESH 0.05

/*
* Displacement data for Z axis (Bicycle moving forwards / backwards)
* forward = z < 0
* backward = z > 0
*/
struct displacementData {
    float currSpeed;
    float distTravelled;
    uint32_t millisBefore;
};

void bmiSetup();
void bmiTask(void *parameter);
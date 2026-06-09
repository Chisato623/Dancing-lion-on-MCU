#include "motor.h"
#include "board.h"

/* Convert 0-180 degree to timer CCP value, matches motor/empty.c */
uint16_t ang(uint8_t a)
{
    if (a > 180) a = 180;
    return 9000 + ((uint32_t)a * 1000) / 180;
}

/* Set all 4 servos by angle, matches motor/empty.c */
void servo_set(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3)
{
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a0), GPIO_PWM_MOTOR2_C1_IDX);//front_left  → PA22
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a1), GPIO_PWM_MOTOR2_C3_IDX);//front_right → PA17
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a2), GPIO_PWM_MOTOR2_C0_IDX);//behind_left → PA21
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a3), GPIO_PWM_MOTOR2_C2_IDX);//behind_right→ PA15
}

/* Initialize: all servos to 90° (mid position) */
void servo_zero(void)
{
    servo_set(90, 90, 90, 90);
    delay_ms(500);
}

/* Sit pose: (90, 90, 45, 45), matches motor/empty.c */
void sit(void)
{
    servo_set(90, 90, 45, 45);
    delay_ms(500);
}

/* Stand pose: (90, 90, 90, 90), matches motor/empty.c */
void stand(void)
{
    servo_set(90, 90, 90, 90);
    delay_ms(500);
}

/* Walking gait parameters */
#define GAIT_CENTER       90
#define GAIT_AMPLITUDE    40
#define GAIT_STEP_DELAY   15
#define GAIT_PERIOD_MS    800

/* Gait phase accumulator (global, reset on new gait start) */
static uint32_t gait_t = 0;

/* Reset gait phase */
void reset_gait(void)
{
    gait_t = 0;
}

/* Continuous walking gait: diagonal trot with triangular wave phase modulation.
   Call this function repeatedly in a loop to produce continuous forward motion.
   Front_Left & Behind_Right move together, Front_Right & Behind_Left move together,
   180° out of phase. */
void robot_forward(void)
{
    uint32_t half = GAIT_PERIOD_MS / 2;
    int aA, aB;

    /* Phase A: 0 → 1 → 0 triangular wave (starts at 0) */
    float phaseA = (float)(gait_t % GAIT_PERIOD_MS) / GAIT_PERIOD_MS;
    if (phaseA < 0.5f)
        phaseA = 2.0f * phaseA;
    else
        phaseA = 2.0f * (1.0f - phaseA);

    /* Phase B: same triangular wave shifted by half period (180° out of phase) */
    float phaseB = (float)((gait_t + half) % GAIT_PERIOD_MS) / GAIT_PERIOD_MS;
    if (phaseB < 0.5f)
        phaseB = 2.0f * phaseB;
    else
        phaseB = 2.0f * (1.0f - phaseB);

    /* Map phase [0,1] to angle [CENTER-AMPLITUDE, CENTER+AMPLITUDE] */
    aA = GAIT_CENTER + (int)((phaseA - 0.5f) * 2 * GAIT_AMPLITUDE);
    aB = GAIT_CENTER + (int)((phaseB - 0.5f) * 2 * GAIT_AMPLITUDE);

    /* Clamp angles */
    if (aA < 0) aA = 0;
    if (aA > 180) aA = 180;
    if (aB < 0) aB = 0;
    if (aB > 180) aB = 180;

    /* Diagonal pair: (FL, BR) = group A, (FR, BL) = group B */
    servo_set((uint8_t)aA, (uint8_t)aB, (uint8_t)aB, (uint8_t)aA);

    delay_ms(GAIT_STEP_DELAY);
    gait_t += GAIT_STEP_DELAY;
}

/* Dance motion: front legs do walking gait, rear legs oscillate between
   fixed extreme positions (50° ←→ 130°) for a swaying dance effect. */
void dance(void)
{
    uint32_t half = GAIT_PERIOD_MS / 2;
    int aA, aB;

    /* Phase A: triangular wave */
    float phaseA = (float)(gait_t % GAIT_PERIOD_MS) / GAIT_PERIOD_MS;
    if (phaseA < 0.5f)
        phaseA = 2.0f * phaseA;
    else
        phaseA = 2.0f * (1.0f - phaseA);

    /* Phase B: shifted triangular wave */
    float phaseB = (float)((gait_t + half) % GAIT_PERIOD_MS) / GAIT_PERIOD_MS;
    if (phaseB < 0.5f)
        phaseB = 2.0f * phaseB;
    else
        phaseB = 2.0f * (1.0f - phaseB);

    aA = GAIT_CENTER + (int)((phaseA - 0.5f) * 2 * GAIT_AMPLITUDE);
    aB = GAIT_CENTER + (int)((phaseB - 0.5f) * 2 * GAIT_AMPLITUDE);

    if (aA < 0) aA = 0;
    if (aA > 180) aA = 180;
    if (aB < 0) aB = 0;
    if (aB > 180) aB = 180;

    /* Front legs: walking pattern (aA, aB)
       Rear legs: extreme sway (50°, 130°) */
    servo_set((uint8_t)aA, (uint8_t)aB, 50, 130);

    delay_ms(GAIT_STEP_DELAY);
    gait_t += GAIT_STEP_DELAY;
}
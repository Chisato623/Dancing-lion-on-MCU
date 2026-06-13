#include "motor.h"
#include "board.h"

#define SERVO_MIN_ANGLE        45U
#define SERVO_CENTER_ANGLE     90U
#define SERVO_MAX_ANGLE        135U
#define SERVO_REAR_LEFT_SIT_ANGLE    125U
#define SERVO_REAR_RIGHT_SIT_ANGLE    55U

#define WALK_STEP_DELAY_MS     80U
#define WALK_FRAME_COUNT       8U
#define DANCE_STEP_DELAY_MS    140U

typedef struct {
    uint8_t front_left;
    uint8_t front_right;
    uint8_t behind_left;
    uint8_t behind_right;
} ServoPose_t;

static uint32_t gait_t = 0;

static uint8_t clamp_servo_angle(uint8_t angle)
{
    if (angle < SERVO_MIN_ANGLE) {
        return SERVO_MIN_ANGLE;
    }
    if (angle > SERVO_MAX_ANGLE) {
        return SERVO_MAX_ANGLE;
    }
    return angle;
}

uint16_t ang(uint8_t a)
{
    a = clamp_servo_angle(a);
    return 9000U + ((uint32_t)a * 1000U) / 180U;
}

void servo_set(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3)
{
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a0), GPIO_PWM_MOTOR2_C1_IDX); /* front left: PA22 */
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a1), GPIO_PWM_MOTOR2_C3_IDX); /* front right: PA17 */
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a2), GPIO_PWM_MOTOR2_C0_IDX); /* behind left: PA21 */
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR2_INST, ang(a3), GPIO_PWM_MOTOR2_C2_IDX); /* behind right: PA15 */
}

void servo_zero(void)
{
    servo_set(SERVO_CENTER_ANGLE, SERVO_CENTER_ANGLE,
              SERVO_CENTER_ANGLE, SERVO_CENTER_ANGLE);
    delay_ms(500);
}

void sit(void)
{
    servo_set(SERVO_CENTER_ANGLE, SERVO_CENTER_ANGLE,
              SERVO_REAR_LEFT_SIT_ANGLE, SERVO_REAR_RIGHT_SIT_ANGLE);
    delay_ms(500);
}

void stand(void)
{
    servo_zero();
}

void reset_gait(void)
{
    gait_t = 0;
}

void robot_forward(void)
{
    static const ServoPose_t walk_table[WALK_FRAME_COUNT] = {
        { 90U,  90U,  90U,  90U},
        { 75U, 105U, 125U,  55U},
        { 65U, 115U, 125U,  55U},
        { 75U, 105U, 125U,  55U},
        { 90U,  90U,  90U,  90U},
        {105U,  75U,  55U, 125U},
        {115U,  65U,  55U, 125U},
        {105U,  75U,  55U, 125U},
    };

    const ServoPose_t *pose = &walk_table[gait_t % WALK_FRAME_COUNT];

    servo_set(pose->front_left, pose->front_right,
              pose->behind_left, pose->behind_right);

    gait_t++;
    delay_ms(WALK_STEP_DELAY_MS);
}

void dance(void)
{
    static const ServoPose_t dance_table[] = {
        { 65U, 115U, SERVO_REAR_LEFT_SIT_ANGLE, SERVO_REAR_RIGHT_SIT_ANGLE},
        { 90U,  90U, SERVO_REAR_LEFT_SIT_ANGLE, SERVO_REAR_RIGHT_SIT_ANGLE},
        {115U,  65U, SERVO_REAR_LEFT_SIT_ANGLE, SERVO_REAR_RIGHT_SIT_ANGLE},
        { 90U,  90U, SERVO_REAR_LEFT_SIT_ANGLE, SERVO_REAR_RIGHT_SIT_ANGLE},
    };
    const uint32_t frame_count = sizeof(dance_table) / sizeof(dance_table[0]);
    const ServoPose_t *pose = &dance_table[gait_t % frame_count];

    servo_set(pose->front_left, pose->front_right,
              pose->behind_left, pose->behind_right);

    gait_t++;
    delay_ms(DANCE_STEP_DELAY_MS);
}

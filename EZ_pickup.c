#pragma config(Sensor, S2,     Gyro,           sensorEV3_Gyro)
#pragma config(Sensor, S3,     Color,          sensorEV3_Color)
#pragma config(Sensor, S4,     Ultra,          sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          claw,          tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          left,          tmotorEV3_Large, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorC,          right,         tmotorEV3_Large, PIDControl, reversed, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define ENCODER_10CM (10/17.6)*360
#define DEFAULT_SPEED 20
#define CORRECTION_RATE 1.1
#define GYRO_CORRECTION 1.0

//move with speed
void move(int speedB, int speedC) {
	setMotorSpeed(motorB, -1*speedB);
	setMotorSpeed(motorC, -1*speedC);
}

//move with speed for time
void moveTime(int speedB, int speedC, int time) {
	move(speedB, speedC);
	delay(time);
	move(0, 0);
}

//move the claw
void moveMotorA(int speed, int ms) {
	setMotorSpeed(motorA, speed);
	delay(ms);
	setMotorSpeed(motorA, 0);
}

//pick up or set down
void claw(bool pickUp) {
	if (pickUp == 1) {
		setMotorSpeed(motorA, -40);
		delay(1000);
		setMotorSpeed(motorA, 0);
		} else {
		setMotorSpeed(motorA, 40);
		delay(1000);
		setMotorSpeed(motorA, 0);
	}
}

//turn x degrees
void xturnDegrees(int degrees) {
	resetGyro(S2);
	int correction = 100;

	if (getGyroDegrees(S2) <= degrees*GYRO_CORRECTION) {
		correction *= -1;
	}

	while (abs(getGyroDegrees(S2)) < abs(degrees*GYRO_CORRECTION)) {
		setMotorSync(motorB, motorC, correction, 15);
	}

	move(0, 0);
}

// initialization
void init() {
	moveMotorA(100, 600);
	resetGyro(S2);
}

//search
void search() {
	int dist = 255;
	int color_reflect = 0;
	bool busy = true;
	resetGyro(S2);
	int heading = getGyroDegrees(S2);
	int dHeading = 0;

	while (busy) {
		dist = getUSDistance(S4);
		color_reflect = getColorReflected(S3);
		dHeading = getGyroDegrees(S2) - heading;
		setMotorSync(motorB, motorC, -1*dHeading*CORRECTION_RATE, -1*DEFAULT_SPEED);

		if (dist < 10) {
			moveTime(-5, -20, 3500);
			resetGyro(S2);
		} else if (color_reflect > 10) {
			move(0, 0);
			setMotorSyncEncoder(motorB, motorC, -1*dHeading*CORRECTION_RATE, ENCODER_10CM, -1*DEFAULT_SPEED);
			delay(750);
			claw(true);
			busy = false;
		} else {
			setMotorSync(motorB, motorC, -1*dHeading*CORRECTION_RATE, -1*DEFAULT_SPEED);
		}
	}
}

// return to base
void returnToBase() {
	xturnDegrees(180);
	claw(false);
	moveTime(-40, -40, 1000);
	xturnDegrees(-180);
}

// sort item
void sortItem() {
	//TBD
}

task main() {
	init();
	while(true) {
		search();
		returnToBase();
		sortItem();
	}
}

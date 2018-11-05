#pragma config(Sensor, S1,     Linefinder,     sensorEV3_Color, modeEV3Color_RGB_Raw)
#pragma config(Sensor, S3,     Color,          sensorEV3_Color, modeEV3Color_RGB_Raw)
#pragma config(Sensor, S4,     Ultra,          sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          claw,          tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          left,          tmotorEV3_Large, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorC,          right,         tmotorEV3_Large, PIDControl, reversed, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define ENCODER_10CM (10/17.6)*360
#define COLOR_THRESHOLD 36
#include "EV3Mailbox.c"
#define DEFAULT_TURN_SPEED 20
#define DEFAULT_LINE_SPEED 25
#define BEND_SPEED (int)((DEFAULT_SPEED/3)*2);

int DEFAULT_SPEED = 25;
bool go = true;
bool evenLane = true;
int numSorted = 0;

#define SLINE S1
#define SCOLOR S3
#define SULTRA S4

int calBlue = 0;
int calGreen = 0;
int calYellow = 0;

//move with speed
void move(int speedB, int speedC) {
	setMotorSpeed(motorB, speedB);
	setMotorSpeed(motorC, speedC);
}

//pick up or set down
void clawControl(bool pickUp) {
	if (pickUp == 1) {
		setMotorSpeed(motorA, -40);
		delay(1000);
		setMotorSpeed(motorA, 0);
		} else {
		setMotorSpeed(motorA, 40);
		delay(1000);
		setMotorSpeed(motorA, -40);
		delay(470);
		setMotorSpeed(motorA, 0);
	}
}

void calibrate() {
	sensorReset(SCOLOR);
	delay(200);
	getColorHue(SCOLOR);
	delay(500);
	eraseDisplay();
	displayBigTextLine(1, "SHOW BLUE");
	waitUntil(getButtonPress(buttonEnter));
	calBlue = getColorHue(SCOLOR);
	delay(300);
	eraseDisplay();
	displayBigTextLine(1, "SHOW YELLOW");
	waitUntil(getButtonPress(buttonEnter));
	calYellow = getColorHue(SCOLOR);
	delay(300);
	eraseDisplay();
	displayBigTextLine(1, "SHOW GREEN");
	waitUntil(getButtonPress(buttonEnter));
	calGreen = getColorHue(SCOLOR);
	delay(300);

}

// true=right, false=left
int turnTicks = 200;
void turn90(bool direction) {
	move(0, 0);
	if(!direction) {
		setMotorSyncEncoder(motorB, motorC, 100, turnTicks, DEFAULT_TURN_SPEED);
		} else {
		setMotorSyncEncoder(motorB, motorC, -100, turnTicks, DEFAULT_TURN_SPEED);
	}
	waitUntilMotorStop(motorB);
}

bool followLineRight() {
	if(getColorReflected(SLINE) < COLOR_THRESHOLD) {
		motor(motorC) = BEND_SPEED;
		motor(motorB) = DEFAULT_LINE_SPEED;
		} else {
		motor(motorC) = DEFAULT_LINE_SPEED;
		motor(motorB) = BEND_SPEED;
	}
	return true;
}

//string getColorFromHue(int hue) {
//	if(hue > 0 && hue < ){return "red";}
//	else if(){return "yellow";}
//	else if(){return "green";}
//	else if(){return "cyan";}
//	else if(){return "blue";}
//	else if(){return "purple";}
//	else if(){return "red";}
//	else {return null;}
//}

// listen to commands from computer
bool TListen() {
	char msgBufIn[MAX_MSG_LENGTH];  // To contain the incoming message.
	openMailboxIn("EV3_INBOX0");
	waitForMailboxMessage(0);
	readMailboxIn("EV3_INBOX0", msgBufIn);
	while(!(msgBufIn[0] == 'S') && !(msgBufIn[2] == 'a')){

	}
	int newspeed = ((int)msgBufIn[6])-48;
	switch(newspeed)
	{
		case 1:
		DEFAULT_SPEED = 5;
		break;
		case 2:
		DEFAULT_SPEED = 10;
		break;
		case 3:
		DEFAULT_SPEED = 15;
		break;
		case 4:
		DEFAULT_SPEED = 20;
		break;
		case 5:
		DEFAULT_SPEED = 25;
		break;
		default:
		DEFAULT_SPEED = 100;
		break;
	}

	return go;
}

void sendBack(int num) {
	char msgBufOut[MAX_MSG_LENGTH];  // To contain the outgoing message
	openMailboxOut("EV3_OUTBOX0");
	sprintf(msgBufOut, "%i", num);
	writeMailboxOut("EV3_OUTBOX0", msgBufOut);

}

// initialization
void init() {
	calibrate();
	eraseDisplay();
	displayBigTextLine(1, "READY");
	waitUntil(getButtonPress(buttonEnter));
	eraseDisplay();
	clawControl(false);
	move(0, 0);

	displayBigTextLine(2, "WAITING FOR");
	displayBigTextLine(5, "CONNECTION");
	while(!TListen()) {
		delay(100);
	}
	eraseDisplay();
}


//search
bool search() {
	int dist = 255;
	int color_reflect = 0;
	bool busy = true;
	delay(100);
	sensorReset(SCOLOR);
	delay(100);
	bool done = false;
	evenLane = true;

	while (busy) {
		dist = getUSDistance(SULTRA);
		color_reflect = getColorReflected(SCOLOR);
		move(DEFAULT_SPEED, DEFAULT_SPEED);
		string out = "";

		if (dist < 14) {
			bool turn = true;
			if (!evenLane) {
				turn = false;
			}

			if (dist < 5) {
				setMotorSyncTime(motorB, motorC, 0, 500, DEFAULT_SPEED);
			}
			turn90(turn);
			setMotorSyncEncoder(motorB, motorC, 0, -0.8*ENCODER_10CM, -1*DEFAULT_SPEED);
			waitUntilMotorStop(motorB);
			turn90(turn);
			evenLane = !evenLane;
		}

		if (color_reflect > 4) {
			move(0, 0);
			setMotorSyncEncoder(motorB, motorC, 0, ENCODER_10CM, -0.6*DEFAULT_SPEED);
			delay(750);
			clawControl(true);
			busy = false;
			} else {
			move(DEFAULT_SPEED, DEFAULT_SPEED);
		}
	}

	if (done) {
		return false;
		} else {
		return true;
	}
}

// return to base
void returnToBase() {
	int turn = true;
	if (!evenLane) {
		turn = false;
	}

	turn90(turn);
	sensorReset(SLINE);
	delay(100);
	while (getUSDistance(SULTRA) > 12) {
		move(DEFAULT_SPEED, DEFAULT_SPEED);
	}

	setMotorSyncEncoder(motorB, motorC, 0, -0.2*ENCODER_10CM, -1*DEFAULT_SPEED);
	waitUntilMotorStop(motorB);
	setMotorSyncEncoder(motorB, motorC, 0, 1.5*ENCODER_10CM, DEFAULT_SPEED);
	turn90(true);
	sensorReset(SLINE);

	while(followLineRight()) {
		if(getUSDistance(SULTRA) < 15) {
			setMotorSyncEncoder(motorB, motorC, 0, -0.8*ENCODER_10CM, -DEFAULT_SPEED);
			waitUntilMotorStop(motorB);
			turn90(true);
			waitUntilMotorStop(motorB);
			break;
		} else {}
	}
}

// sort item
void sortItem() {
	displayBigTextLine(1, "SORTING");
	delay(300);
	sensorReset(SCOLOR);
	delay(200);
	getColorHue(SCOLOR);
	delay(500);
	int toyColor = getColorHue(SCOLOR);
	int distance = 0;

	eraseDisplay();
	if ((toyColor > calBlue - 10) && (toyColor < calBlue + 10)) {
		displayBigTextLine(4, "BLUE");
		distance = 2000;

	} else if ((toyColor > calYellow - 10) && (toyColor < calYellow + 10)) {
		displayBigTextLine(4, "YELLOW");
		distance = 3700;

	} else if ((toyColor > calGreen - 10) && (toyColor < calGreen + 10)) {
		displayBigTextLine(4, "GREEN");
		distance = 6000;

	}  else {
		displayBigTextLine(4, "BLUE");
		distance = 2000;
	}

	clearTimer(T2);
	while(followLineRight()) {
		if(getTimerValue(T2) > distance) {
			break;
		}
	}

	turn90(false);
	clawControl(false);
	setMotorSyncEncoder(motorB, motorC, 0, 0.8*ENCODER_10CM, DEFAULT_SPEED);
	waitUntilMotorStop(motorB);
	turn90(true);
	while(followLineRight()) {
		if (getUSDistance(SULTRA) < 5) {
			break;
		}
	}
	turn90(true);
	numSorted++;

}

task main() {
	init();
	bool searching = true;
	while(searching) {
		searching = search();
		returnToBase();
		sortItem();
		sendBack(numSorted);
		resetAllSensorAutoID();
	}
}
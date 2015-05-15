
void Mcontrol(char _comm, int _speed, int _dir){
	switch(_comm){
				// mybe subtract dir from speed to make this leaner?
	case 'F': //fwd

		analogWrite(drv_pins[1],_speed - _dir );	// fwd right
		analogWrite(drv_pins[2],_speed - _dir );	// fwd left
						
		analogWrite(drv_pins[0],0); // bwd right
		analogWrite(drv_pins[3],0); // bwd left
		break;
	case 'B': //bwd
		analogWrite(drv_pins[0],_speed - _dir);	// bwd left
		analogWrite(drv_pins[3],_speed - _dir);	// bwd right
						
		analogWrite(drv_pins[1],0); // bwd right
		analogWrite(drv_pins[2],0); // bwd left
		break;
	case 'R': //release
		for(int i=0;i<4;i++)
			analogWrite(drv_pins[i],0);		
		break;
	case 'H': //hold
		for(int i=0;i<4;i++)
			analogWrite(drv_pins[i],255);
		break;
	}

}

void Mdir(char _comm, int _pwm){
	switch(_comm){
	case 'L': //fwd
		analogWrite(drv_pins[2],_pwm);	// fwd left
		analogWrite(drv_pins[0],_pwm);	// bwd right
		break;
	case 'R': //bwd
		analogWrite(drv_pins[2],_pwm);	// bwd left
		analogWrite(drv_pins[0],_pwm);	// bwd right
		break;
	case 'H': //release
		for(int i=0;i<4;i++)
			analogWrite(drv_pins[i],0);		
		break;
	}
}


void Mspeed(char _comm, int _speed){
	switch(_comm){
				// mybe subtract dir from speed to make this leaner?
	case 'F': //fwd

		analogWrite(drv_pins[1],_speed );	// fwd right
		analogWrite(drv_pins[2],_speed );	// fwd left
						
		analogWrite(drv_pins[0],0); // bwd right
		analogWrite(drv_pins[3],0); // bwd left
		break;
	case 'B': //bwd
		analogWrite(drv_pins[0],_speed);	// bwd left
		analogWrite(drv_pins[3],_speed);	// bwd right
						
		analogWrite(drv_pins[1],0); // bwd right
		analogWrite(drv_pins[2],0); // bwd left
		break;
	case 'R': //release
		for(int i=0;i<4;i++)
			analogWrite(drv_pins[i],0);		
		break;
	case 'H': //hold
		for(int i=0;i<4;i++)
			analogWrite(drv_pins[i],255);
		break;
	}

}


//dir test
//digitalWrite(drv_pins[0],HIGH); // bwd right
//digitalWrite(drv_pins[1],HIGH); // fwd right

//digitalWrite(drv_pins[2],HIGH); // fwd left
//digitalWrite(drv_pins[3],HIGH); // bwd left
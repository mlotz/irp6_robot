#include <rtt/Component.hpp>

#include "Irp6pmM2J.h"
#include "Irp6pmTransmission.h"


Irp6pmM2J::Irp6pmM2J(const std::string& name) : RTT::TaskContext(name, PreOperational) {

	this->ports()->addPort("MotorPosition", port_motor_position_);
	this->ports()->addPort("JointPosition", port_joint_position_);

	this->addProperty("synchro_motor_position", synchro_motor_position_);
}

Irp6pmM2J::~Irp6pmM2J() {

}

bool Irp6pmM2J::configureHook() {
	motor_position_.resize(NUMBER_OF_SERVOS);
	joint_position_.resize(NUMBER_OF_SERVOS);
	return true;
}

void Irp6pmM2J::updateHook() {
	port_motor_position_.read(motor_position_);
	mp2i(&motor_position_(0), &joint_position_(0));	
	port_joint_position_.write(joint_position_);
}

void Irp6pmM2J::mp2i(const double* motors, double* joints)
{
  // zmienne pomocnicze
  double c, d, l;
  double sinus, cosinus;
  double M1, M2;

// Przelicznik polozenia walu silnika napedowego kolumny w radianach
// na kat obrotu kolumny (wspolrzedna wewnetrzna) w radianach
  joints[0] = (motors[0] - synchro_motor_position_[0]) / GEAR[0] + THETA[0];

// Przelicznik polozenia walu silnika napedowego ramienia dolnego w radianach
// na kat obrotu ramienia (wspolrzedna wewnetrzna) w radianach
  l = (motors[1] - synchro_motor_position_[1]) / GEAR[1] + THETA[1];
  M1 = mi1 * mi1 + ni1 * ni1;
  c = l * l - sl123;
  d = sqrt(M1 - c * c);
  cosinus = (mi1 * c - ni1 * d) / M1;
  sinus = -(ni1 * c + mi1 * d) / M1;
  joints[1] = atan2(sinus, cosinus);

// Przelicznik polozenia walu silnika napedowego ramienia gornego w radianach
// na kat obrotu ramienia (wspolrzedna wewnetrzna) w radianach
  l = (motors[2] - synchro_motor_position_[2]) / GEAR[2] + THETA[2];
  M2 = mi2 * mi2 + ni2 * ni2;
  c = l * l - sl123;
  d = sqrt(M2 - c * c);
  cosinus = (mi2 * c - ni2 * d) / M2;
  sinus = -(ni2 * c + mi2 * d) / M2;
  joints[2] = atan2(sinus, cosinus);

// Przelicznik polozenia walu silnika napedowego obrotu kisci T w radianach
// na kat pochylenia kisci (wspolrzedna wewnetrzna) w radianach
  joints[3] = (motors[3] - synchro_motor_position_[3]) / GEAR[3];

// Przelicznik polozenia walu silnika napedowego obrotu kisci V w radianach
// na kat obrotu kisci (wspolrzedna wewnetrzna) w radianach
  joints[4] = (motors[4] - synchro_motor_position_[4] - (motors[3]
                             - synchro_motor_position_[3])) / GEAR[4] + THETA[4];

// Przelicznik polozenia walu silnika napedowego obrotu kisci N w radianach
// na kat obrotu kisci (wspolrzedna wewnetrzna) w radianach
  joints[5] = (motors[5] - synchro_motor_position_[5]) / GEAR[5] + THETA[5];

  joints[2] -= joints[1] + M_PI_2;
  joints[3] -= joints[2] + joints[1] + M_PI_2;

}

ORO_CREATE_COMPONENT(Irp6pmM2J)


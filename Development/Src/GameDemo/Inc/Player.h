class Player : public VObject
{
	DECLARE_CLASS(Player, VObject);

public:
	Player(): m_value(3){}
	virtual ~Player();

	INT m_value;
};

class Monster : public Player
{
	DECLARE_CLASS(Monster, Player);

public:
	Monster() : m_val(33){}
	virtual ~Monster();

	INT m_val;
};

void foo();
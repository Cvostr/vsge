class Player : ZPScript{

	Entity@ _this;

	Player(Entity@ entity)
	{
		@_this = entity;
	}
	
	void Start() {
		_this.SetName("scripted");
		_this.SetActive(false);
		print("OnStart");
	}
	void Update(){
		print(Vec3(12, 13, 14));
	}
}
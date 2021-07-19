class Player : ZPScript{

	Entity@ _this;

	Player(Entity@ entity)
	{
		@_this = entity;
	}
	
	void Start() {
		_this.SetName("scripted");
		_this.SetActive(false);
	}
	void Update(){
		
	}
}
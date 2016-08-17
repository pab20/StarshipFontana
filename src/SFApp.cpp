#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetHP(300)//set health points
  player->SetPosition(player_pos);
  cout <<"player HP "<<player->HP()<<endl;

 
  const int number_of_walls = 1; 
  for(int w=0; w<number_of_walls; w++) {
//number_of_walls * w
   auto wall      = make_shared<SFAsset>(SFASSET_WALL, sf_window);
   auto wall_pos2 = Point2((canvas_w/number_of_walls) * w, 200.0f);
   auto wall_pos3 = Point2(canvas_w/number_of_walls, 400.0f);
   wall->SetPosition(wall_pos2);
   wall->SetPosition(wall_pos3);
}

 const int number_of_aliens = 10;
  for(int j=0; j<number_of_aliens; j++) {
    //number_of_aliens * j
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/number_of_aliens) * j, 200.0f);
	auto pos2 = Point2((canvas_w / number_of_aliens) * j, 400.0f);
    alien->SetPosition(pos);
	alien->SetPosition(pos2);
    alien->SetHP(60);
    aliens.push_back(alien);
  }

 const int number_of_coins = 5;
  for(int c=0; c<number_of_coins; c++) {
//number_of_coins * c
  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/number_of_coins)* c, 100.0f);
  coin->SetPosition(pos);
  coins.push_back(coin);
  }
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_PLAYER_UP:
    player->GoNorth();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
    int w, h;
    SDL_GetRendererOutputSize(sf_window -> getRenderer(), &w, &h);
    //Update projectile positions
    for(auto p: projectiles) {
    p->GoNorth();
    auto p_pos=p->GetPosition();
    if(p_pos.getY()>h){p->HandleCollision();
     }
   }

  for(auto c: coins) {
    c->GoSouth();
    if (player->CollidesWith(c)){
	c->HandleCollision();
	cout<<"COIN GAINED"<<endl;
	player->SetPoints(player->Points() +20);
    }
  }

  // Update enemy positions
  for(auto a : aliens) 
    a->GoEast();
    // do something here
   if(player-> CollidesWith(a)){
	   player->SetHP(player->HP()-30);
	  cout<<player->HP()<<" HP OUTSTANDING"<<endl;
	  
	 if(player->HP()<=0){
	cout<<"GAMEOVER!"<<" Total Score: "<<player->Points()<<endl;
	is_running=false;//it is the end of the game once the players health reaches zero
  }
}

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        cout<<"Enemy ELIMINATED!"<<endl;
        p->HandleCollision();
        a->HandleCollision();
      }
    }
  }

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
}
   list<shared_ptr<SFAsset>> p_remove;
   for(auto p : projectiles) {
    if(p->IsAlive()) {
      p_remove.push_back(p);
    }
  }
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(p_remove);


  list<shared_ptr<SFAsset>> coin_remove;
  for(auto c : coins) {
    if(c->IsAlive()) {
      coin_remove.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(coin_remove);

}
void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());
  // draw the player
  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender(); 
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}

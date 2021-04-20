#define OLC_PGE_APPLICATION

#include <string>

#include "olcPixelGameEngine.h"

enum class mapTiles { None, Wall, Road, Road_LEdge, Road_REdge, Road_TEdge, Road_BEdge };

class Game : public olc::PixelGameEngine
{
public:
  Game() { sAppName = "Frazzer Racing"; }

private:
  const float ROT_RATE   = 2.0f;
  const float ACCEL_RATE = 50.f;
  const float FRICTION   = 25.f;
  const float PI         = 3.14159f;

  olc::vf2d carPos    = { 200, 200 };
  float     carVel    = 0.0f;
  float     fCarAngle = 0.0f;

  std::unique_ptr<olc::Sprite> sprCar;
  std::unique_ptr<olc::Decal>  decCar;
  std::unique_ptr<olc::Sprite> sprTiles;
  std::unique_ptr<olc::Decal>  decTiles;
  std::unique_ptr<mapTiles[]>  map;
  olc::vi2d                    vBlockSize = { 10, 10 };

public:
  bool OnUserCreate() override
  {
    map = std::make_unique<mapTiles[]>( ScreenWidth() * ScreenHeight() / 10 );
    for( int y = 0; y < ScreenHeight() / 10; y++ )
    {
      for( int x = 0; x < ScreenWidth() / 10; x++ )
      {
        if( x == 0 || y == 0 || x == ScreenWidth() / 10 - 1 || y == ScreenHeight() / 10 - 1 )
          map[cordToIndex( x, y )] = mapTiles::Wall;
        else
          map[cordToIndex( x, y )] = mapTiles::None;
      }
    }

    // Load Sprites
    sprCar   = std::make_unique<olc::Sprite>( "./gfx/car.png" );
    sprTiles = std::make_unique<olc::Sprite>( "./gfx/mapTiles.png" );

    // Create decals
    decCar = std::make_unique<olc::Decal>( sprCar.get() );

    return true;
  }

  bool OnUserUpdate( float fElapsedTime ) override
  {
    // Get User input
    if( GetKey( olc::Key::A ).bHeld ) fCarAngle -= ROT_RATE * fElapsedTime;
    if( GetKey( olc::Key::D ).bHeld ) fCarAngle += ROT_RATE * fElapsedTime;
    if( GetKey( olc::Key::W ).bHeld ) carVel += ACCEL_RATE * fElapsedTime;
    if( GetKey( olc::Key::S ).bHeld ) carVel -= ACCEL_RATE * fElapsedTime;

    // Car friction
    if( carVel > 0 ) carVel -= FRICTION * fElapsedTime;
    else if( carVel < 0 )
      carVel += FRICTION * fElapsedTime;

    // Update Car
    olc::vf2d vel = { sin( fCarAngle ) * carVel, -cos( fCarAngle ) * carVel };
    carPos += vel * fElapsedTime;

    // Make Sure Angle Stays in range
    if( fCarAngle < 0 ) { fCarAngle = 2 * PI - fCarAngle; }
    else if( fCarAngle > 2 * PI )
    {
      fCarAngle = fCarAngle - 2 * PI;
    }

    // Keep car on screen
    if( carPos.x < 0 ) carPos.x = 0;
    if( carPos.x > ScreenWidth() ) carPos.x = (float)ScreenWidth();
    if( carPos.y < 0 ) carPos.y = 0;
    if( carPos.y > ScreenHeight() ) carPos.y = (float)ScreenHeight();

    Clear( olc::VERY_DARK_GREY );

    // Draw Map
    for( int y = 0; y < ScreenHeight() / 10; y++ )
    {
      for( int x = 0; x < ScreenWidth() / 10; x++ )
      {
        switch( map[cordToIndex( x, y )] )
        {
          case mapTiles::None: break;
          case mapTiles::Wall:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 0, 0 ) * vBlockSize,
                               vBlockSize );
            break;
        }
      }
    }

    // Draw Car
    DrawRotatedDecal( carPos, decCar.get(), fCarAngle, { 5.0f, 10.0f } );

    DrawString( 0, 0, std::to_string( fCarAngle ) );
    DrawString( 0, 8, std::to_string( carVel ) );
    DrawString( 0,
                16,
                std::to_string( sin( fCarAngle ) * carVel ) + " " + std::to_string( -cos( fCarAngle ) * carVel ) );

    return true;
  }

  int cordToIndex( olc::vi2d pos ) { return cordToIndex( pos.x, pos.y ); }
  int cordToIndex( int x, int y ) { return y * ScreenWidth() + x; }
};

int main()
{
  Game demo;
  if( demo.Construct( 800, 400, 2, 2 ) ) demo.Start();
  return 0;
}
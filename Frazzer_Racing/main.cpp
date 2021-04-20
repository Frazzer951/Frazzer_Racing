#define OLC_PGE_APPLICATION

#include <fstream>
#include <string>

#include "olcPixelGameEngine.h"

enum class mapTiles {
  None,
  Wall,
  Road,
  Road_L_Edge,
  Road_R_Edge,
  Road_T_Edge,
  Road_B_Edge,
  Road_TL_Corner,
  Road_TR_Corner,
  Road_BL_Corner,
  Road_BR_Corner
};

class Game : public olc::PixelGameEngine
{
public:
  Game() { sAppName = "Frazzer Racing"; }

private:
  const float ROT_RATE   = 2.0f;
  const float ACCEL_RATE = 50.f;
  const float FRICTION   = 25.f;
  const float PI         = 3.14159f;

  olc::vf2d carPos    = { 130, 200 };
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
    map = std::make_unique<mapTiles[]>( ScreenWidth() * ScreenHeight() / vBlockSize.x );
    for( int y = 0; y < ScreenHeight() / vBlockSize.y; y++ )
    {
      for( int x = 0; x < ScreenWidth() / vBlockSize.x; x++ )
      {
        if( x == 0 || y == 0 || x == ScreenWidth() / vBlockSize.x - 1 || y == ScreenHeight() / vBlockSize.y - 1 )
          map[cordToIndex( x, y )] = mapTiles::Wall;
        else if( x >= 10 && x <= 70 && y >= 5 && y <= 9 )
          map[cordToIndex( x, y )] = mapTiles::Road;
        else if( x >= 10 && x <= 70 && y >= 30 && y <= 34 )
          map[cordToIndex( x, y )] = mapTiles::Road;
        else if( x >= 10 && x <= 15 && y >= 5 && y <= 34 )
          map[cordToIndex( x, y )] = mapTiles::Road;
        else if( x >= 65 && x <= 70 && y >= 5 && y <= 34 )
          map[cordToIndex( x, y )] = mapTiles::Road;
        else if( x >= 10 && x <= 70 && ( y == 4 || y == 29 ) )
          map[cordToIndex( x, y )] = mapTiles::Road_T_Edge;
        else if( x >= 10 && x <= 70 && ( y == 10 || y == 35 ) )
          map[cordToIndex( x, y )] = mapTiles::Road_B_Edge;
        else if( ( x == 9 || x == 64 ) && y >= 5 && y <= 34 )
          map[cordToIndex( x, y )] = mapTiles::Road_L_Edge;
        else if( ( x == 16 || x == 71 ) && y >= 5 && y <= 34 )
          map[cordToIndex( x, y )] = mapTiles::Road_R_Edge;
        else if( ( x == 9 && y == 4 ) )
          map[cordToIndex( x, y )] = mapTiles::Road_TL_Corner;
        else if( ( x == 9 && y == 35 ) )
          map[cordToIndex( x, y )] = mapTiles::Road_BL_Corner;
        else if( ( x == 71 && y == 4 ) )
          map[cordToIndex( x, y )] = mapTiles::Road_TR_Corner;
        else if( ( x == 71 && y == 35 ) )
          map[cordToIndex( x, y )] = mapTiles::Road_BR_Corner;
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
    for( int y = 0; y < ScreenHeight() / vBlockSize.y; y++ )
    {
      for( int x = 0; x < ScreenWidth() / vBlockSize.x; x++ )
      {
        switch( map[cordToIndex( x, y )] )
        {
          case mapTiles::None: FillRect( olc::vi2d( x, y ) * vBlockSize, vBlockSize, olc::DARK_GREEN ); break;
          case mapTiles::Wall:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 0, 0 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 0, 1 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_T_Edge:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 3, 0 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_B_Edge:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 3, 1 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_L_Edge:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 4, 1 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_R_Edge:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 4, 0 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_TL_Corner:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 1, 0 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_TR_Corner:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 2, 0 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_BL_Corner:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 1, 1 ) * vBlockSize,
                               vBlockSize );
            break;
          case mapTiles::Road_BR_Corner:
            DrawPartialSprite( olc::vi2d( x, y ) * vBlockSize,
                               sprTiles.get(),
                               olc::vi2d( 2, 1 ) * vBlockSize,
                               vBlockSize );
            break;
        }
      }
    }

    // Draw Car
    DrawRotatedDecal( carPos, decCar.get(), fCarAngle, { 5.0f, 10.0f } );

    DrawString( 11, 11, std::to_string( fCarAngle ) );
    DrawString( 11, 20, std::to_string( carVel ) );
    DrawString( 11,
                29,
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
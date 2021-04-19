#define OLC_PGE_APPLICATION

#include <string>

#include "olcPixelGameEngine.h"

template<class _Ty>
void clamp( _Ty & val, _Ty min, _Ty max )
{
  if( min > max )
  {
    _Ty temp = min;
    min      = max;
    max      = temp;
  }
  val = ( val < min ) ? min : ( val > max ) ? max : val;
}

class Game : public olc::PixelGameEngine
{
public:
  Game() { sAppName = "Frazzer Racing"; }

private:
  const float ROT_RATE   = 2.0f;
  const float ACCEL_RATE = 0.01f;
  const float PI         = 3.14159f;

  olc::vf2d carPos    = { 200, 200 };
  float     carVel    = 0.0f;
  float     fCarAngle = 0.0f;

  std::unique_ptr<olc::Sprite> sprCar;
  std::unique_ptr<olc::Decal>  decCar;

public:
  bool OnUserCreate() override
  {
    // Load Fragment Sprite
    sprCar = std::make_unique<olc::Sprite>( "./gfx/car.png" );

    // Create decal of fragment
    decCar = std::make_unique<olc::Decal>( sprCar.get() );

    return true;
  }

  bool OnUserUpdate( float fElapsedTime ) override
  {
    // Get User input
    if( GetKey( olc::Key::A ).bHeld ) fCarAngle -= ROT_RATE * fElapsedTime;
    if( GetKey( olc::Key::D ).bHeld ) fCarAngle += ROT_RATE * fElapsedTime;
    if( GetKey( olc::Key::W ).bHeld ) carVel += ACCEL_RATE;
    if( GetKey( olc::Key::S ).bHeld ) carVel -= ACCEL_RATE;

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
    if( carPos.x > ScreenWidth() ) carPos.x = ScreenWidth();
    if( carPos.y < 0 ) carPos.y = 0;
    if( carPos.y > ScreenHeight() ) carPos.y = ScreenHeight();

    Clear( olc::VERY_DARK_GREY );
    // Draw Car
    DrawRotatedDecal( carPos, decCar.get(), fCarAngle, { 5.0f, 10.0f } );

    DrawString( 0, 0, std::to_string( fCarAngle ) );
    DrawString( 0, 8, std::to_string( carVel ) );
    DrawString( 0,
                16,
                std::to_string( sin( fCarAngle ) * carVel ) + " " + std::to_string( -cos( fCarAngle ) * carVel ) );

    return true;
  }
};

int main()
{
  Game demo;
  if( demo.Construct( 400, 400, 2, 2 ) ) demo.Start();
  return 0;
}
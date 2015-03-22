#ifndef OPTION_MENU_H_INCLUDED
#define OPTION_MENU_H_INCLUDED

#include "global.h"

//This is quite disgusting, but just bear with it.
class Option_Manager
{
private:
  //defaults
  TextureImage caret;                       //the blinking line in text
  TextureImage default_text_background;     //a long bar
  //time step
  TextureImage Timestep_Text;               //option description
  TextureImage Timestep_Prompt_Background;  //input text background
  TextureImage Timestep_Input_Text;         //input text
  //collision rects/points
  TextureImage Collision_Rects_Text;
  TextureImage Collision_Rects_Red_Button;
  TextureImage Collision_Rects_Green_Button;

  //fake lag
  int Timestep_Value;
  std::string input_text;
  int text_selected;
  bool text_update;
  bool blink;
  std::chrono::steady_clock::time_point blink_time;
  //collision rect
  bool toggle;

public:

  bool open = false;

  Option_Manager()
  {
    text_update = false; blink = false; Timestep_Value = timestep; text_selected = -1; toggle = false;
    blink_time = std::chrono::steady_clock::now();

    Timestep_Text.loadFromRenderedText("Time step: ",{255,255,255},Font);
    caret.FillRect(2,Timestep_Text.getHeight(),{0,0,0,255});
    default_text_background.FillRect(400,Timestep_Text.getHeight(),{0,100,100,200});
    Timestep_Prompt_Background.FillRect(100,Timestep_Text.getHeight(),{255,255,255,255});
    std::stringstream ss; ss<<timestep;
    Timestep_Input_Text.loadFromRenderedText(ss.str(),{0,0,0},Font);

    Collision_Rects_Text.loadFromRenderedText("Draw collision_rects: ",{255,255,255},Font);
    Collision_Rects_Red_Button.FillRect(25,25,{255,0,0,255});
    Collision_Rects_Green_Button.FillRect(25,25,{0,255,0,255});
  }

  //not much
  void logic()
  {
    if(text_selected != -1 && std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::steady_clock::now() - blink_time).count() > 500){
      blink = !blink;
      blink_time = std::chrono::steady_clock::now();
    }

    if(text_update){
      //switch(text_selected){
      //case Option_Time_Step:
        if(!input_text.empty())
          Timestep_Input_Text.loadFromRenderedText(input_text,{0,0,0},Font);
        else
          Timestep_Input_Text.loadFromRenderedText(" ",{0,0,0},Font);
       //break;
      //}
    }
  }

  void render()
  {
    //sorry for all the random numbers, but its pretty strait forward.

    //fake lag
    default_text_background.render(10,10);
    Timestep_Text.render(10,10);
    Timestep_Prompt_Background.render(10+Timestep_Text.getWidth(),10);
    Timestep_Input_Text.render(10+Timestep_Text.getWidth(),10);
    if(blink && text_selected != -1){
      switch(text_selected){
      case Option_Time_Step:
        if(!input_text.empty())
          caret.render(10+Timestep_Text.getWidth()+Timestep_Input_Text.getWidth(),10);
        else
          caret.render(10+Timestep_Text.getWidth(),10); //at the beginning
        break;
      }
    }
    //collision rects
    default_text_background.render(10,20+Timestep_Text.getHeight());
    Collision_Rects_Text.render(10,20+Timestep_Text.getHeight());
    if(!toggle)
      Collision_Rects_Red_Button.render(default_text_background.getWidth()-Collision_Rects_Red_Button.getWidth()
                                  ,25+Timestep_Text.getHeight());
    else
      Collision_Rects_Green_Button.render(default_text_background.getWidth()-Collision_Rects_Red_Button.getWidth()
                                  ,25+Timestep_Text.getHeight());
  }

  void input() //check input
  {
    //if we turn it off
    if( e.type == SDL_KEYUP && e.key.repeat == 0 && e.key.keysym.sym == SDLK_TAB ){
      Option_Manager::exit();
      open = false;
    }

    if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
      //anything that was selected to update
      switch(text_selected){ //redundant atm.
      case Option_Time_Step:
        std::istringstream iss(input_text); iss >> Timestep_Value;
        if(Timestep_Value <= 0 || Timestep_Value > 1000 )
          { std::cout<<"invalid input("<<Timestep_Value<<")\n"; Timestep_Value = timestep; input_text = std::to_string(timestep); text_update = true; }
        text_selected = -1;
        break;
      }

      if( MouseOver({10+Timestep_Text.getWidth(),10,100,Timestep_Text.getHeight()})){
        text_selected = Option_Time_Step;
        input_text = ""; //blank
        text_update = true;
      }

      //draw collisions button
      if(MouseOver({default_text_background.getWidth()-Collision_Rects_Red_Button.getWidth()
                                  ,25+Timestep_Text.getHeight(),25,25})){
        toggle = !toggle;
      }
    }

    if(text_selected != -1) {
      if( e.type == SDL_KEYDOWN) {
        if(e.key.keysym.sym == SDLK_RETURN) {
          if(!input_text.empty()) {
            switch(text_selected) { //redundant atm.
            case Option_Time_Step:
              std::istringstream iss(input_text); iss >> Timestep_Value;
              if(Timestep_Value <= 0 || Timestep_Value > 1000 )
                { std::cout<<"invalid input("<<Timestep_Value<<")\n"; Timestep_Value = timestep; input_text = std::to_string(timestep); text_update = true;  }
              break;
            }
          }
          text_selected = -1;
        }

        //Handle backspace
        if( e.key.keysym.sym == SDLK_BACKSPACE && input_text.length() != 0 ) {
          //lop off character
          input_text.pop_back();
          text_update = true;
        }
      }

      //Handle copy
      else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL ) {
        SDL_SetClipboardText( input_text.c_str() );
      }
      //Handle paste
      else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL ) {
        input_text = SDL_GetClipboardText();
        text_update = true;
      }

      //Special text input event
      else if( e.type == SDL_TEXTINPUT ) {
        //Not copy or pasting
        if( !( ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' )
              && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' )
              && SDL_GetModState() & KMOD_CTRL ) ) {
          //Append character
          input_text += e.text.text;
          text_update = true;
        }
      }
    }
  }

  //implement changes.
  void exit()
  {
    text_update = false; blink = false; text_selected = -1; //reset

    std::istringstream iss(input_text); iss >> Timestep_Value;
    if(Timestep_Value <= 0 || Timestep_Value > 2000 )
      { std::cout<<"invalid input("<<Timestep_Value<<")\n"; Timestep_Value = timestep; input_text = std::to_string(timestep); text_update = true; }

    timestep = Timestep_Value;
    draw_collision_rects = toggle;

    collision_rects.clear(); //just in case.
  }
};

#endif // OPTION_MENU_H_INCLUDED

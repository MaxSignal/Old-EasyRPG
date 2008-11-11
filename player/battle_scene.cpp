/*battle_scene.cpp, battle scene routines.
   Copyright (C) 2007 EasyRPG Project <http://easyrpg.sourceforge.net/>.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <vector>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "math-sll.h"
#include "sprite.h"
#include "key.h"
#include "font.h"
#include "audio.h"
#include "map.h"
#include "skill.h"
#include "item.h"
#include "enemy.h"
#include "player.h"
#include "actor.h"
#include "scene.h"

void Battle_scene::init(Audio *the_audio, bool *run, Uint8 *the_scene,Player_team *the_team)
{
    My_team     = the_team;
    My_audio    = the_audio;
    the_run     = run;
    (*My_audio).music_load("../Music/Battle2.mid");
    title.x_pos     = 0;
    title.y_pos     = 0;
    title.set_image("../Backdrop/Grass.png");

    Window_text.init(320, 80,0, 160);
    Window_text.visible = false;
    update_window_stats();
    // menu_os.init( the_audio, run, 0,4, 96, 80, 96, 160);
    // menu_os.visible = false;
    My_menu.init( the_audio, run, 0,4, 96, 80, 0, 160);
    string_vector.push_back("Attack");
    string_vector.push_back("Skills");
    string_vector.push_back("Objects");
    string_vector.push_back("Defend");
    string_vector.push_back("Escape");
    My_menu.set_commands(&string_vector);
    running         = run;
    new_scene       = the_scene;
    menu_command    my_menu_command;
    int i;
    for (i = 0;i<My_team->get_size();i++) //tantos comandos como jugadores.
    menu_commands.push_back(my_menu_command);
    state               = 0;
    new_menu_used       = 0;
    player_in_turn      = 0;
    monster_in_turn     = 0;
    player_turns        = 0;
    update_window_monster_select();
    monster_select.visible = false;
}
void Battle_scene::update_window_stats()
{
    Window.init(My_audio,the_run,0,3,224,80,96,160,214,16 );
    int i = 0;
    char string_buffer[255];
    for (i = 0;i<My_team->get_size();i++)
    {
        sprintf(string_buffer, "Hp %d / %d  Mp %d ", (*My_team->get_hp(i)), (*My_team->get_max_hp(i)),(*My_team->get_mp(i)));
        Window.add_text(string_buffer,110, 5+(i*16));
        Window.add_text((My_team->get_name(i)),10,5+(i*16));
        if ((*My_team->get_hp(i))>0)
            Window.add_text("Normal",60, 5+(i*16));
        else
            Window.add_text("Muerto",60, 5+(i*16));
    }

}

void Battle_scene::window_text_show_damage(bool type,int atak,int ataked,int damage)
{
    char string_buffer[255];
    sprintf(string_buffer, "%d HP perdidos ",damage );
    Window_text.init(320, 80,0, 160);

    if (type)//si son los players
    {
        Window_text.add_text((My_team->get_name(atak)),5,5);//name heroe
        Window_text.add_text("ataca al enemigo",70, 5);
        Window_text.add_text(((My_team->enemies.at(ataked)).get_name()),5, 25);//name moustruo
        Window_text.add_text(string_buffer,70, 25);
    }
    else
    {
        Window_text.add_text(((My_team->enemies.at(atak)).get_name()),5, 5);//name moustruo
        Window_text.add_text("ataca ",70, 5);
        Window_text.add_text((My_team->get_name(ataked)),5,25);//name heroe
        Window_text.add_text(string_buffer,70, 25);
    }




}

void Battle_scene::update_window_monster_select()
{
    int i,j,k = 0;
    j=My_team->enemies.size();

    for (i = 0;i<j;i++)  //dibuja todos los monster
    {
        if ( (*(My_team->enemies.at(i)).get_hp())>0)//cambiar por arreglo
        {
            string_vector2.push_back((My_team->enemies.at(i)).get_name());
            k++;
        }
    }
    monster_select.init( My_audio, the_run, 0,k-1, 96, 80, 0, 160);
    monster_select.set_commands(&string_vector2);

}


void Battle_scene::update(SDL_Surface*screen)
{
    int i,j;
    SDL_FillRect(screen, NULL, 0x0);// Clear screen
    j=My_team->enemies.size();
    title.draw(screen);
    Window.draw(screen);
    My_menu.draw(screen);
    monster_select.draw(screen);
    Window_text.draw(screen);
    for (i = 0;i<j;i++)  //dibuja todos los monster
        ((My_team->enemies.at(i)).battler).draw(screen);
    if (state == 1)//si le toca alos heroes
        {
            attack(screen,player_in_turn,menu_commands.at(player_in_turn).selected_monster);
        }
    if (state == 2)//si le toca a los moustruos
        {
            attacked(monster_in_turn);
        }
}

void Battle_scene::win()
{
    Uint32 i;
    int k = 0;  //  S.O.S

    for (i = 0; i < (My_team->enemies).size(); i++)
    {
        if(*(My_team->enemies.at(i).get_hp()) == 0)     //cambiar por arreglo
        {
            ((My_team->enemies.at(i)).battler).visible = false;//haz que ya no se vea
            k++;
        }
    }
    if (k==My_team->get_size())//si todos los enemigos muetros
    {
        *new_scene = 1;//   return to map scene
    }
}
void Battle_scene::lose()
{
    int i   = 0;
    int k   = 0;

    for (i = 0; i < My_team->get_size(); i++)
    {
        if ((My_team->get_hp(i))==0) // in case doesn't works, put a pointer behind My_team( "(My_team->" = "(*My_team->" )
        {
            k++;
        }
    }
    if (k==My_team->get_size())   //  if all heroes are dead:
        {
            *new_scene = 3;//game over
        }

}


void Battle_scene::attack(SDL_Surface*screen,int new_chara,int enemy)
{
    int damage;
    while ((My_team->enemies.at(enemy).get_hp()) == 0)   //     if doesn't works, see rev.128 or olders
    {
        enemy++;    //  it makes choose another enemy
        enemy = (enemy % (My_team->enemies).size());
    }

    (*(My_team->get_weapon_anim(new_chara))).x_pos=((My_team->enemies.at(enemy)).battler).x_pos - (((My_team->enemies.at(enemy)).battler).get_weight()) / 2;
    (*(My_team->get_weapon_anim(new_chara))).y_pos=((My_team->enemies.at(enemy)).battler).y_pos - (((My_team->enemies.at(enemy)).battler).get_height()) / 2;
    (*(My_team->get_weapon_anim(new_chara))).draw(screen);


    if (My_team->get_weapon_anim(new_chara)->end_anim)    // if attack finishes. I put a few void lines for avoid stress
    {

        My_team->get_weapon_anim(new_chara)->reset();     //it resets weapon animation for next attack

        damage = ( *(My_team->get_attack(new_chara)) );         //attack damage

        *(My_team->enemies.at(enemy).get_hp()) = *(My_team->enemies.at(enemy).get_hp()) - damage;     //Enemy's health points after attack

        Window_text.dispose();
        window_text_show_damage(true,new_chara,enemy,damage);   //  it shows a game window where damage attack info is printed in screen

        if ((*((My_team->enemies.at(enemy)).get_hp())) < 0)     //  if hp counter of enemies is lower than 0(death)
            {
                (*((My_team->enemies.at(enemy)).get_hp())) = 0;     //  puts the hp counter of enemies to 0 to avoid errors
            }
        if ((player_turns + 1) < My_team->get_size())
        {
            player_in_turn++;   //  it would be a table
            player_turns++;
            win();
        }
        else
        {
            win();
            state   = 2;
        }//it's turn for monsters

    }
}
void Battle_scene::attacked(int enemy)
{
    int i;
    int j;
    static   int xt_pos = title.x_pos, flag = 0,timer = 0,moves = 0;
    static bool finish = false;
    if (((My_team->enemies.at(enemy)).battler).visible)//si esta vivo el enemigo
    {
        timer++;
        if (timer == 4)
        {
            flag++;
            timer = 0;
            if (flag%2)
            {
                title.x_pos = xt_pos + 20;
                j=My_team->enemies.size();
                for (i = 0; i < j; i++)
                    ((My_team->enemies.at(i)).battler).x_pos=((My_team->enemies.at(i)).battler).x_pos+20;
            }
            else
            {
                flag        = 0;
                moves++;
                timer       = 0;
                title.x_pos = xt_pos - 20;
                j           = My_team->enemies.size();
                for (i = 0; i < j; i++)
                {
                    ((My_team->enemies.at(i)).battler).x_pos=((My_team->enemies.at(i)).battler).x_pos - 20;
                }
            }
        }
        if (moves == 10)
        {
            moves       = 11;
            flag        = 0;
            timer       = 10;
            title.x_pos = xt_pos;
            j=My_team->enemies.size();

            int         damage;

            // player selection

            int k=(rand() % My_team->get_size());   //random selecion

            while (*(My_team->get_hp(k)) == 0)      //if choosed player is dead
            {
                k++;    //chooses another
                k = (k % My_team->get_size());
            }

            damage = *My_team->enemies.at(enemy).get_attack(); //damage calculum

            *My_team->get_hp(k) = *My_team->get_hp(k) - damage;

            if (*My_team->get_hp(k) < 0)
                *My_team->get_hp(k) = 0;

            lose();
            Window_text.dispose();
            window_text_show_damage(false,enemy ,k,damage);
            finish = true;

        }


        if (finish)
        {
            if (timer == 120)
            {
                moves   = 0;
                flag    = 0;
                timer   = 0;
                finish  = false;
                j=My_team->enemies.size();

                if (monster_in_turn + 1 < j)
                {
                    monster_in_turn++;
                }
                else
                {
                    state = 0;
                    give_turn();//le toca a los comandos
                }
            }

        }

    }
    else  //si el enemigo esta muerto
    {

        moves   = 0;
        flag    = 0;
        timer   = 0;
        finish  = false;
        j=My_team->enemies.size();
        if (monster_in_turn + 1 < j)//si aun hay moustruos <-- traducir
        {
            monster_in_turn++;    //another monster turn
        }
        else
        {
            state = 0;  //  battle reset
            give_turn();//le toca a los comandos <-- traducir
        }
    }
}
void Battle_scene::give_turn()
{
    monster_select.dispose();
    update_window_monster_select();
    Window.dispose();
    update_window_stats();
    My_menu.restart_menu();
    My_menu.visible = true;
    Window_text.visible = false;
    monster_select.visible = false;
    Window.visible_window = true;
    new_menu_used = 0;
}
void Battle_scene::action_monster_select()
{
    int i,j;
    j=My_team->enemies.size();
    for (i = 0;i<j;i++)
        if (monster_select.get_index_y()==i)
        {
            menu_commands.at(new_menu_used).selected_monster = i;
            new_menu_used++;
            monster_select.restart_menu();
            monster_select.visible = false;
            My_menu.visible = true;
        }

    if (new_menu_used==My_team->get_size())//ya todos eligieron
    {
        state = 1;
        My_menu.visible = false;
        Window_text.visible = true;
        Window.visible_window = false;
        player_in_turn = 0;//no heroes a husado ningun turno
        monster_in_turn = 0;//los moustruos tampo han usad
        player_turns = 0;
    }
}

void Battle_scene::action()
{
    int i;
    //My_menu_commands.at(num).des2
    if (My_menu.visible)
    {
        for (i = 0;i<4;i++)
            if (My_menu.get_index_y()==i)
            {
                menu_commands.at(new_menu_used).des1 = i;

                My_menu.restart_menu();
                My_menu.visible = false;
                monster_select.visible = true;
            }


        if (My_menu.get_index_y()==4)
        {
            // state = 1;
            *new_scene = 1;
        }
    }
}

void Battle_scene::update_key()
{


    if (new_menu_used<My_team->get_size())//si aun no han elegido todos
    {

        if (monster_select.visible)
        {
            monster_select.update_key();
            if (monster_select.decision())
                action_monster_select();
        }
        if (My_menu.visible)
        {
            My_menu.update_key();
            if (Window.visible!=true)//que se vea que perso elige
                Window.visible = true;
            Window.set_y_cursor((16*new_menu_used) +5);//posicionado en el perso
            if (My_menu.decision())
                action();
        }


    }
}
void Battle_scene::dispose()
{
    title.dispose();
    Window.dispose();
    My_team->clear_enemy();
    (*My_audio).stop_music();
    My_menu.dispose();
}

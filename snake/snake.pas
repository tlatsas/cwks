(************************************************)
(*                  S N @ K E                   *)
(*                                              *)
(*                  (c) 2005                    *)
(*                Tasos Latsas                  *)
(*                                              *)
(************************************************)

PROGRAM snake;

{ windows }
{ USES WinCrt, WinDos; }

{ linux }
USES Crt, Dos;

TYPE
    FRAME = array[1..15] of STRING;
    DIRECTION = (up, down, left, right);
    DIFFICULTY_LEVEL = (easy, normal, hard, godlike);
    NUM_MATRIX_ALLOWED_VALUES = 0..4000000;
    NUM_MATRIX_TYPE = packed array[1..15, 1..47] of NUM_MATRIX_ALLOWED_VALUES;
    SMATRIX = array[1..18] of STRING; {this matrix is used for credits}

VAR
   nest:           FRAME;
   course:         DIRECTION;
   difficulty:     DIFFICULTY_LEVEL;
   difficulty_str: STRING;

   game_over, quit, subexit: BOOLEAN;

   X_egg, Y_egg, head_X_snake, head_Y_snake, tail_X_snake, tail_Y_snake: INTEGER;
   dir_X_head, dir_Y_head, prev_tail_X_snake, prev_tail_Y_snake:         INTEGER;

   egg_counter, selection, subselection, DLevel, add_body_part_value, body_length: BYTE;

   score, sum_egg_counter:      WORD;
   button, SnakeBody, decision: CHAR;

   head_number, last_bp_number: NUM_MATRIX_ALLOWED_VALUES;

   NumMatrix:  NUM_MATRIX_TYPE;
   delay_time: INTEGER;

   credits_table: SMATRIX;

{==========================================================================}

PROCEDURE DELAY(delay_time_val: INTEGER);
var
    hour, minute, second, sec100 : word;
    init_sec100, cur_sec100      : word;
Begin
    GetTime(hour, minute, second, sec100);
    init_sec100 := sec100;
    repeat
        GetTime(hour, minute, second, sec100);
        cur_sec100 := sec100;
    until (abs(cur_sec100 - init_sec100) > delay_time_val)
end; { delay }

{==========================================================================}

PROCEDURE CONTINUE;
begin
     write('- press any key to continue -');
     readkey;
     CLRSCR;
end;

{==========================================================================}

PROCEDURE RANDOM_EGG(var x, y: INTEGER);
begin
     nest[x, y]:=' ';
     repeat
           x:=random(13)+2;
           y:=random(45)+2;
     until nest[x, y]<>SnakeBody;
     nest[x, y]:='o';
     gotoXY(y, x);
     write('o');
end; { random_egg }

{==========================================================================}

PROCEDURE DISPLAY_BOARD;
var
   i: INTEGER;
begin
     { nest[15,47] }
     nest[1]  := '===============================================';
     nest[2]  := '|                                             |';
     nest[3]  := '|                                             |';
     nest[4]  := '|                                             |';
     nest[5]  := '|                                             |';
     nest[6]  := '|                                             |';
     nest[7]  := '|                                             |';
     nest[8]  := '|                                             |';
     nest[9]  := '|                                             |';
     nest[10] := '|                                             |';
     nest[11] := '|                                             |';
     nest[12] := '|                                             |';
     nest[13] := '|                                             |';
     nest[14] := '|                                             |';
     nest[15] := '===============================================';

     for i:=1 to 15 do
         writeln(nest[i]);
end; { display_board }

{==========================================================================}

PROCEDURE ADD_BODY_PART_TO_TAIL;
begin

     gotoXY(tail_Y_snake, tail_X_snake);

     nest[tail_X_snake, tail_Y_snake]:=SnakeBody;
     write(SnakeBody);

     last_bp_number:=last_bp_number-1;

     NumMatrix[tail_X_snake, tail_Y_snake]:=last_bp_number;

     tail_X_snake:=prev_tail_X_snake;
     tail_Y_snake:=prev_tail_Y_snake;

end; { add_body_part_to_tail }

{==========================================================================}

PROCEDURE LOCATE_TAIL;
begin
     if NumMatrix[tail_X_snake, tail_Y_snake-1] = last_bp_number then
        tail_Y_snake := tail_Y_snake-1
     else
         if NumMatrix[tail_X_snake, tail_Y_snake+1] = last_bp_number then
            tail_Y_snake := tail_Y_snake+1
         else
             if NumMatrix[tail_X_snake-1, tail_Y_snake] = last_bp_number then
                tail_X_snake := tail_X_snake-1
             else
                 if NumMatrix[tail_X_snake+1, tail_Y_snake] = last_bp_number then
                    tail_X_snake := tail_X_snake+1;
end; { locate_tail }

{==========================================================================}

PROCEDURE MOVEMENT;
begin
     LOCATE_TAIL;
     gotoXY(tail_Y_snake, tail_X_snake);              { erase     }
     nest[tail_X_snake, tail_Y_snake]:=' ';           { the last  }
     write(' ');                                      { body part }

     last_bp_number:=last_bp_number+1;

     NumMatrix[tail_X_snake, tail_Y_snake]:=0;         { zero number table block at tail }


     gotoXY(head_Y_snake, head_X_snake);               { rewrite   }
                                                       { body part }
     nest[head_X_snake, head_Y_snake]:=SnakeBody;      { at the    }
     write(SnakeBody);                                 { beginning }


     head_number:=head_number+1;                          { increasing head }
     NumMatrix[head_X_snake, head_Y_snake]:=head_number;  { number          }

     head_X_snake:= head_X_snake + dir_X_head;
     head_Y_snake:= head_Y_snake + dir_Y_head;

end; {movement}

{==========================================================================}

PROCEDURE SHIFT_DIRECTION (button_value: CHAR);
begin
     if ((button_value='w') or (button_value='W')) and (course<>down) then
        begin
             dir_X_head:=-1;
             dir_Y_head:=0;
             course:=up;
        end
     else
         if ((button_value='s') or (button_value='S')) and (course<>up) then
            begin
                 dir_X_head:=1;
                 dir_Y_head:=0;
                 course:=down;
            end
         else
             if ((button_value='a') or (button_value='A')) and (course<>right) then
                begin
                     dir_X_head:=0;
                     dir_Y_head:=-1;
                     course:=left;
                end
             else
                 if ((button_value='d') or (button_value='D')) and (course<>left) then
                    begin
                         dir_X_head:=0;
                         dir_Y_head:=1;
                         course:=right;
                    end;
end; { shift_direction }

{==========================================================================}

PROCEDURE INITIAL_PLACEMENT;
begin
     nest[8, 26]:=SnakeBody;
     NumMatrix[8, 26]:=5;

     nest[8, 27]:=SnakeBody;
     NumMatrix[8, 27]:=4;

     nest[8, 28]:=SnakeBody;
     NumMatrix[8, 28]:=3;

     nest[8, 29]:=SnakeBody;
     NumMatrix[8, 29]:=2;

     nest[8, 30]:=SnakeBody;
     NumMatrix[8, 30]:=1;

     head_number:=5;
     last_bp_number:=NumMatrix[8, 30]; { last_bp_number = 1 }

     gotoXY(26, 8);
     write(SnakeBody);
     write(SnakeBody);
     write(SnakeBody);
     write(SnakeBody);
     write(SnakeBody);
     head_X_snake:=8;
     head_Y_snake:=25;
     course:=left;

     tail_X_snake:=8;
     tail_Y_snake:=31;

end; { initial_placement }

{==========================================================================}

PROCEDURE NUM_M_VAL(var NumMatrix_reference: NUM_Matrix_TYPE);
var
   i, j: INTEGER;
begin
     for i:=1 to 15 do
         for j:=1 to 47 do
             NumMatrix_reference[i, j]:=0;
end; { num_t_val }

{==========================================================================}

FUNCTION SCORE_SYSTEM (sum_egg_cnt_value: WORD): BYTE;
begin
     case sum_egg_cnt_value of
            1..10  : SCORE_SYSTEM := 5;
           11..25  : SCORE_SYSTEM := 8;
           26..35  : SCORE_SYSTEM := 16;
           36..50  : SCORE_SYSTEM := 28;
           51..75  : SCORE_SYSTEM := 40;
           76..99  : SCORE_SYSTEM := 60;
          100..135 : SCORE_SYSTEM := 80;
          136..170 : SCORE_SYSTEM := 90;
       else          SCORE_SYSTEM := 100;
     end; { case }
end; { score_system }

{==========================================================================}

PROCEDURE WELCOME;
var
   sign: ARRAY[1..11] OF STRING;
   i: INTEGER;
begin
     sign[1]  := '/////////////////////';
     sign[2]  := '/                   //////////////';
     sign[3]  := '/                                /';
     sign[4]  := '/ !!!!! !!   ! @@@@@ !  !  !!!!! /';
     sign[5]  := '/ !     ! !  ! @   @ ! !   !     /';
     sign[6]  := '/  !!!  !  ! ! @@@@@ ! !   !!!   /';
     sign[7]  := '/     ! !   !! @   @ !  !  !     /';
     sign[8]  := '/ !!!!! !    ! @   @ !   ! !!!!! /';
     sign[9]  := '/                                /';
     sign[10] := '////////////////////    ver_1.9b /';
     sign[11] := '                   ///////////////';

     for i:=1 to 11 do
         begin
              DELAY(10);
              gotoXY(20, 2+i);
              writeln(sign[i]);
         end;
     gotoXY(22, 20);
     write('- press any key to continue -');
     readkey;
     CLRSCR;
end; { welcome }

{==========================================================================}

PROCEDURE INSTRUCTIONS;
begin
     CLRSCR;
     writeln(' Try to feed the snake with the eggs to grow');
     writeln('  BE AWARE: don`t bite your tail or collide');
     writeln('           with the sharp wall! ;)');
     writeln;
     writeln(' You move the snake with the following keys');
     writeln;
     writeln('                 W/w -> up');
     writeln('         down <- S/s');
     writeln('                 D/d -> right');
     writeln('         left <- A/a');
     writeln;writeln;writeln;
     writeln('             HAVE FUN !!!');
     gotoXY(2, 19);
     writeln('- press any key to return to main menu -');
     readkey;
     CLRSCR;
end; { instructions }

{==========================================================================}

PROCEDURE DISPLAY_MENU (var selection_ref: BYTE);
begin
     writeln('                ******************************************');
     writeln('                *          WELCOME   TO   SN@KE          *');
     writeln('                ******************************************');
     writeln('                *                                        *');
     writeln('                *           1. START GAME                *');
     writeln('                *                                        *');
     writeln('                *           2. HOW TO PLAY               *');
     writeln('                *                                        *');
     writeln('                *           3.   OPTIONS                 *');
     writeln('                *                                        *');
     writeln('                *           4.   CREDITS                 *');
     writeln('                *                                        *');
     writeln('                *           5.    QUIT                   *');
     writeln('                *                                        *');
     writeln('                ******************************************');
     writeln;
     write('SELECTION >');
     readln(selection_ref);
end; {display menu}

{==========================================================================}

PROCEDURE DISPLAY_OPTIONS_MENU (var subselection_ref: BYTE);
begin
     writeln('                ******************************************');
     writeln('                *                                        *');
     writeln('                *         1.  DIFFICULTY LEVEL           *');
     writeln('                *                                        *');
     writeln('                *         2.   SNAKE SETTINGS            *');
     writeln('                *                                        *');
     writeln('                *         3. RETURN TO MAIN MENU         *');
     writeln('                *                                        *');
     writeln('                *                                        *');
     writeln('                ******************************************');
     writeln;
     write('SELECTION >');
     readln(subselection_ref);
end; {DISPLAY_OPTIONS_MENU}

{==========================================================================}

PROCEDURE DISPLAY_OPTIONS_DLEVEL (var DLevel_ref: BYTE);
begin
     writeln('                ******************************************');
     writeln('                *                                        *');
     writeln('                *         1.   BORING   [easy ;)]        *');
     writeln('                *                                        *');
     writeln('                *         2.   NORMAL   [default]        *');
     writeln('                *                                        *');
     writeln('                *         3.    HARD                     *');
     writeln('                *                                        *');
     writeln('                ******************************************');
     writeln;
     write('SELECTION >');
     readln(DLevel_ref);
end; { display_options_dlevel }

{==========================================================================}

PROCEDURE SNAKE_SETTINGS;
var
   LegalSB: BOOLEAN;
begin
     CLRSCR;
     writeln('                ******************************************');
     writeln('                *                                        *');
     writeln('                *   HERE YOU CAN SELECT HOW THE SNAKE`S  *');
     writeln('                *     BODY WILL APPEAR AT YOUR SCREEN    *');
     writeln('                *        ( THE DEFAULT SHAPE IS @ )      *');
     writeln('                *                                        *');
     writeln('                *    PLEASE TYPE THE DESIRED CHARACTER   *');
     writeln('                *                                        *');
     writeln('                ******************************************');
     repeat
           writeln;
           write('CHARACTER:');
           readln(SnakeBody);
           if (SnakeBody='o') or (SnakeBody='=') or (SnakeBody='|') then
              begin
                   LegalSB:=false;
                   writeln('Unable to use character!Please select again');
              end
           else
               LegalSB:=true;
     until (LegalSB);
     writeln('Changes made successfully');
     CONTINUE;
end; {snake_settings}

{==========================================================================}

PROCEDURE ROLL_CREDITS (var credits_table_ref: SMATRIX);
var
   i, j, n, b: INTEGER;
begin
     n:=1;
     b:=1;
     i:=18;
     while i>=1 do
           begin
                CLRSCR;
                if i>1  then
                   j:=1
                else
                    if b<=8 then
                       begin
                            b:=b+1;
                            j:=b;
                            i:=2;
                       end
                    else
                        i:=0;
                gotoXY(1,i);
                while j<=n do
                      begin
                           writeln(credits_table_ref[j]);
                           j:=j+1;
                      end;
                delay(45);
                If n<=22 then
                   n:=n+1;
                i:=i-1;
           end;{while}
end;{roll_credits}

{==========================================================================}

PROCEDURE BUILD_CREDITS (var c_tbl_ref: SMATRIX); {credits_table}
begin
     c_tbl_ref[1]  :='                ***************************************************';
     c_tbl_ref[2]  :='                *                   C R E D I T S                 *';
     c_tbl_ref[3]  :='                ***************************************************';
     c_tbl_ref[4]  :='                *                   S  N  @  K  E                 *';
     c_tbl_ref[5]  :='                *                                                 *';
     c_tbl_ref[6]  :='                *                version 1.9 (beta)               *';
     c_tbl_ref[7]  :='                *                     (c)2005                     *';
     c_tbl_ref[8]  :='                *                                                 *';
     c_tbl_ref[9]  :='                *                  Tasos Latsas                   *';
     c_tbl_ref[10] :='                *                                                 *';
     c_tbl_ref[11] :='                ***************************************************';
     c_tbl_ref[12] :='';
     c_tbl_ref[13] :='';
     c_tbl_ref[14] :='           This program is free software; you can redistribute it and/or';
     c_tbl_ref[15] :='            modify it under the terms of the GNU General Public License';
     c_tbl_ref[16] :='           as published by the Free Software Foundation; either version 2';
     c_tbl_ref[17] :='               of the License, or (at your option) any later version.';
end;{build_credits}

{==========================================================================}

PROCEDURE TRANSFORM_difVAR2STR (difficulty_val: DIFFICULTY_LEVEL;var difficulty_str_ref: STRING);
begin                                         { This proc transforms the user defined variable }
     if difficulty_val=easy then              {  'difficulty' to string type for printing }
        difficulty_str_ref:='easy'
     else
         if difficulty_val=normal then
            difficulty_str_ref:='normal'
         else
             if difficulty_val=hard then
                difficulty_str_ref:='hard';
end; {TRANSFORM_difVAR2STR}

{==========================================================================}

PROCEDURE UPDATE_STATISTICS (difficulty_str_val: STRING; score_val, sum_egg_counter_val: WORD; body_length_val: BYTE);
begin
     gotoXY(50, 4);
     write('Difficulty: ',difficulty_str_val);
     gotoXY(50, 6);
     write('Current Score: ',score_val,' points');
     gotoXY(50, 8);
     write('Current Eggs: ',sum_egg_counter_val);
     gotoXY(50, 10);
     write('Body Length : 5 + ',body_length_val,' parts');
end; {update_statistics}

{==========================================================================}

BEGIN (* main *)
      quit:=false;
      RANDOMIZE;
      WELCOME;
{default values}
      difficulty:=normal;
      delay_time:=5;
      add_body_part_value:=3;
      SnakeBody:='@';
{main menu loop start}
      while (not quit){=false} do
            begin
                 subexit:=false;        {exit=false for all submenus }
                 DISPLAY_MENU(selection); { displays the main menu & reads the user's selection }

                 case selection of
                      1: begin
                              CLRSCR;
                              DISPLAY_BOARD;
{snake's & egg's first placement}
                              X_egg:=2;   { random starting }
                              Y_egg:=2;   { values          }
                              RANDOM_EGG(X_egg, Y_egg);
                              NUM_M_VAL(NumMatrix);
                              INITIAL_PLACEMENT;
{             initial values }
                              body_length:=0;
                              egg_counter:=0;
                              sum_egg_counter:=0;
                              score:=0;
                              game_over:=FALSE;
                              dir_X_head:=0;
                              dir_Y_head:=-1;
{prepare dif. var for use}
                              TRANSFORM_difVAR2STR (difficulty, difficulty_str);
                              UPDATE_STATISTICS (difficulty_str, score, sum_egg_counter, body_length);
{ main game loop }
                              while game_over=FALSE do
                                    begin
                                         prev_tail_X_snake:=tail_X_snake;
                                         prev_tail_Y_snake:=tail_Y_snake;
                                         MOVEMENT;
                                         DELAY(delay_time);
                                         if nest[head_X_snake, head_Y_snake]='o' then
                                            begin
                                                 egg_counter:=egg_counter + 1;
                                                 sum_egg_counter:= sum_egg_counter + 1;
                                                 score:=score + SCORE_SYSTEM(sum_egg_counter);
                                                 RANDOM_EGG(X_egg, Y_egg);
                                                 UPDATE_STATISTICS (difficulty_str, score, sum_egg_counter, body_length);
                                            end
                                         else
                                             if (nest[head_X_snake, head_Y_snake]='|') then
                                                game_over:=TRUE
                                             else
                                                 if (nest[head_X_snake, head_Y_snake]='=') then
                                                    game_over:=TRUE
                                                 else
                                                     if nest[head_x_snake, head_y_snake]=SnakeBody then
                                                        game_over:=TRUE;
                                         if egg_counter=add_body_part_value then
                                            begin
                                                 egg_counter:=0;
                                                 ADD_BODY_PART_TO_TAIL;
                                                 body_length:=body_length+1;
                                                 UPDATE_STATISTICS (difficulty_str, score, sum_egg_counter, body_length);
                                            end;
                                         if KeyPressed then
                                            begin
                                                 button:=ReadKey;
                                                 SHIFT_DIRECTION(button);
                                            end;
                                    end; {while}
{game over}
                              if game_over then
                                 begin
                                      gotoXY(50, 6);
                                      write('Total Score: ',score,' points   ');
                                      gotoXY(50, 8);
                                      write('Total Eggs: ',sum_egg_counter,'   ');
                                      gotoXY(19, 18);
                                      write('GAME OVER ...');
                                 end;
                              gotoXY(10, 21);
                              write('- hit -ENTER- to continue -');
                              readln;
                              CLRSCR;
                         end; {selection 1 -start game}

                      2: INSTRUCTIONS; {selection 2 -how to play}

                      3: repeat
                              CLRSCR;
                              DISPLAY_OPTIONS_MENU(subselection); { displays the options menu & reads the users selection }

                              case subselection of
                                   1: begin
                                           CLRSCR;
                                           DISPLAY_OPTIONS_DLEVEL (DLevel); { displays the difficulty level menu & }
                                                                            { reads rhe user's selection}
                                           case DLevel of
                                                1: begin
                                                        difficulty:=easy;
                                                        delay_time:=11;
                                                        add_body_part_value:=5;
                                                        writeln('Changes made successfully');
                                                   end;{easy}

                                                2: begin
                                                        difficulty:=normal;
                                                        delay_time:=5;
                                                        add_body_part_value:=3;
                                                        writeln('Changes made successfully');
                                                   end;{normal -default}

                                                3: begin
                                                        difficulty:=hard;
                                                        delay_time:=3;
                                                        add_body_part_value:=1;
                                                        writeln('Changes made successfully');
                                                   end{hard}
                                                else
                                                    writeln('WRONG SELECTION!Try between 1 & 3.');
                                           end; {dlevel_case}
                                           writeln('- press any key to continue -');
                                           readkey;
                                      end; { difficulty level selection}

                                   2: SNAKE_SETTINGS;

                                   3: begin
                                           subexit:=true;
                                           CLRSCR;
                                      end;

                                   else
                                       begin
                                            writeln('WRONG SELECTION!Try between 1 and 3...');
                                            writeln('- press any key to continue -');
                                            readkey;
                                       end;
                              end; {case -options}
                  {exit} until (subexit);
                         { end for selection 3 -options }

                      4: begin
                              BUILD_CREDITS(credits_table);
                              ROLL_CREDITS(credits_table);
                         end;

                      5: quit:=true ;

                      else
                          begin
                               writeln('WRONG SELECTION!Try between 1 and 5...');
                               CONTINUE;
                          end;
                 end; {case -main}
            end; {while -main}
      { WINDOWS }
      { donewincrt; }
END. (* main *)

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "snakes.h"
#include "lwp.h"
#include "util.h"

int AlwaysZero() {
  /* always run the first one */
  return 0;
}

int SIGTSTPcounter = 0;
int ChangeOnSIGTSTP() {
  /* Move to the next one, counting TSTPs*/
  return SIGTSTPcounter%lwp_procs;
}

int ChooseLowestColor(){
  /* choose the lowest available color.  Round-robin within
   * colors.  (I.e. Always advance)
   * We'll do the round-robin within the snake array
   */
  int i,color,next;
  snake s;

  color = MAX_VISIBLE_SNAKE+1;
  for(i=0;i<lwp_procs;i++) {
    s = snakeFromLWpid(lwp_ptable[i].pid);
    if ( s && s->color < color )
      color = s->color;
  }
  next = -1;
  if ( color ){                 /* find the next one of the given color */
    next = (lwp_running+1)%lwp_procs;
    s = snakeFromLWpid(lwp_ptable[next].pid);
    while(s && (s->color != color) ) {
      next = (next+1)%lwp_procs;
      s = snakeFromLWpid(lwp_ptable[next].pid);
    }
  }

  return next;
}

int ChooseHighestColor(){
  /* choose the highest available color.  Round-robin within
   * colors.  (I.e. Always advance)
   * We'll do the round-robin within the snake array
   */
  int i,color,next;
  snake s;

  color = 0;
  for(i=0;i<lwp_procs;i++) {
    s = snakeFromLWpid(lwp_ptable[i].pid);
    if ( s && s->color > color )
      color = s->color;
  }
  next = -1;
  if ( color ){                 /* find the next one of the given color */
    next = (lwp_running+1)%lwp_procs;
    s = snakeFromLWpid(lwp_ptable[next].pid);
    while(s && (s->color != color) ) {
      next = (next+1)%lwp_procs;
      s = snakeFromLWpid(lwp_ptable[next].pid);
    }
  }

  return next;
}


/********************************************************
 * End scheduling algorithms
 * Now the signal handling material
 ********************************************************/
﻿/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* priest.c */
/* functions for clerics. */

#include "glob.h"

/* prayer occurs at altars, hence name of function */
void l_altar(void)
{
  int i,deity;
  char response;

  if (Current_Environment == E_COUNTRYSIDE) deity = DRUID;
  else deity = Level->site[Player.x][Player.y].aux;

  switch(deity) {
    default:
      print1("Tässä primitiivisessä alttarissa ei ole merkintöjä.");
      break;
    case ODIN:
      print1("Tähän graniittialttariin on kaiverrettu vasara.");
      break;
    case SET:
      print1("Tähän hiekkakivialttariin on kaiverrettu salama.");
      break;
    case HECATE:
      print1("Tähän hopeiseen alttariin on kaiverrettu Kokkolintu.");
      break;
    case ATHENA:
      print1("Tähän kultaiseen alttariin on kaiverrettu kukkanen.");
      break;
    case DESTINY:
      print1("Tämä kristallialttari on omegan muotoon kaiverrettu.");
      break;
    case DRUID:
      print1("Tähän tammialttariin on koristeellisesti kaiverrettu lehtiä.");
      break;
  }
  print2("Palvotko tällä alttarilla? [yn] ");
  if (ynq2() == 'y') {

#ifdef INCLUDE_MONKS
    if (Player.rank[MONKS] > 0)
	print2("A monk seeks truth within.");
    else
#endif
    if (Player.rank[PRIESTHOOD] == 0)
      increase_priest_rank(deity);
    else if (! check_sacrilege(deity)) {
      if (Blessing) print1("You have a sense of immanence.");
      print2("Request a Blessing, Sacrifice an item, or just Pray [b,s,p] ");
      do response = (char) mcigetc();
      while ((response != 'b') && 
	     (response != 's') && 
	     (response != 'p') && 
	     (response != ESCAPE));
      if (response == 'b') {
	print1("You beg a heavenly benefice.");
	print2("You hear a gong resonating throughout eternity....");
	morewait();
	if (Blessing) {
	  print1("A shaft of lucent radiance lances down from the heavens!");
	  print2("You feel uplifted....");
	  morewait();
	  gain_experience(Player.rank[PRIESTHOOD]*Player.rank[PRIESTHOOD]*50);
	  cleanse(1);
	  heal(10);
	  bless(1);
	  Blessing = FALSE;
	  increase_priest_rank(deity);
	}
	else {
	  print1("Your ardent plea is ignored.");
	  print2("You feel ashamed.");
	  Player.xp -= (Player.xp/4);
	}
	calc_melee();
      }
      else if (response == 's') {
	i = getitem_prompt("Sacrifice what? ", NULL_ITEM);
	if (i==ABORT) i = 0;
	if (Player.possessions[i] == NULL) {
	  print1("You have insulted your deity!");
	  print2("Not a good idea, as it turns out...");
	  dispel(-1);
	  p_damage(Player.hp-1,UNSTOPPABLE,"a god's pique");
	}
	else if (true_item_value(Player.possessions[i]) >
		 (long) (Player.rank[PRIESTHOOD] *
		  Player.rank[PRIESTHOOD] *
		  Player.rank[PRIESTHOOD] * 50)) {
	  print1("With a burst of blue flame, your offering vanishes!");
	  dispose_lost_objects(1,Player.possessions[i]);
	  print2("A violet nimbus settles around your head and slowly fades.");
	  morewait();
	  Blessing = TRUE;
	}
	else {
	  print1("A darkling glow envelopes your offering!");
	  print2("The glow slowly fades....");
	  morewait();
	  setgamestatus(SUPPRESS_PRINTING);
	  if (Player.possessions[i]->used) {
	    Player.possessions[i]->used = FALSE;
	    item_use(Player.possessions[i]);
	    Player.possessions[i]->blessing =
	      -1 - abs(Player.possessions[i]->blessing);
	    Player.possessions[i]->used = TRUE;
	    item_use(Player.possessions[i]);
	  }
	  else Player.possessions[i]->blessing =
	    -1 - abs(Player.possessions[i]->blessing);
	  resetgamestatus(SUPPRESS_PRINTING);
	}
      }
      else if (response == 'p') {
	if (deity != Player.patron) 
	  print1("Nothing seems to happen.");
	else if (!increase_priest_rank(deity))
	  answer_prayer();
      }
    }
  }
}


int check_sacrilege(int deity)
{
  int i,sacrilege=FALSE;
  if ((Player.patron != deity) && (Player.patron > 0)) {
    sacrilege=TRUE;
    Player.pow--;
    Player.maxpow--;
    switch(Player.patron) {
    case ODIN:
      print1("Ukko Ylijumala notices your lack of faith! ");
      morewait();
      if (deity == ATHENA) {
	print2("However, Mielikki intercedes on your behalf.");
	sacrilege = FALSE;
      }
      else {
	print2("You are struck by a thunderbolt! Sinuun iskee ukkonen!");
	p_damage(Player.level*5,UNSTOPPABLE,"Ukko Ylijumalan raivo, jumalannuoli.");
	if (Player.hp > 0) {
	  morewait();
	  print2("The bolt warps your feeble frame....");
	  Player.maxcon = Player.maxcon/2;
	  Player.con = min(Player.con,Player.maxcon);
	  Player.maxstr = Player.maxstr/2;
	  Player.con = min(Player.str,Player.maxstr);
	}
      }
      morewait();
      break;
    case SET:
      print1("Perkele huomaa uskottomuutesi! ");
      morewait();
      if (deity == HECATE) {
	print1("Mutta koska rukoilet ystävällistä jumaluutta,");
	print2("Perkele päättää olla rankaisematta sinua.");
	sacrilege = FALSE;
      }
      else {
	print2("You are blasted by a shaft of black fire!");
	p_damage(Player.level*5,UNSTOPPABLE,"Perkele's anger");
	if (Player.hp > 0) {
	  morewait();
	  print1("You are wreathed in clouds of smoke.");
	  for(i=0;i<MAXITEMS;i++) 
	    if ((Player.possessions[i] != NULL) &&
		(Player.possessions[i]->blessing > -1))
	      conform_lost_object(Player.possessions[i]);
	  morewait();
	  print2("You feel Perkele's Black Hand on your heart....");
	  Player.con = Player.maxcon = Player.maxcon / 4;
	}
      }
      morewait();
      break;
    case HECATE:
      print1("Louhi notices your lack of faith! ");
      morewait();
      if (deity == SET) {
	print1("But ignores the affront since she likes Perkele.");
	sacrilege = FALSE;
      }
      else {
	print1("You are zapped by dark moonbeams!");
	p_damage(Player.level*5,UNSTOPPABLE,"Louhi's malice");
	if (Player.hp > 0) {
	  print2("The beams leach you of magical power!");
	  Player.maxpow = Player.maxpow/5;
	  Player.pow = min(Player.pow,Player.maxpow);
	  for(i=0;i<NUMSPELLS;i++) 
	    Spells[i].known = FALSE;
	}
      }
      morewait();
      break;
    case ATHENA:
      print1("Mielikki notices your lack of faith! ");
      morewait();
      if (deity == ODIN) {
	print2("But lets you off this time since Ukko Ylijumala is also Lawful.");
	sacrilege = FALSE;
      }
      else {
	print2("You are zorched by godsfire!");
	if (Player.hp > 0) {
	  morewait();
	  print1("The fire burns away your worldly experience!");
		Player.level = 0;
	  Player.xp = 0;
	  Player.maxhp = Player.hp = Player.con;
	  print2("Your power is reduced by the blast!!!");
	  Player.pow = Player.maxpow = Player.maxpow/3;
	  Player.mana = min(Player.mana,calcmana());
	}
      }
      morewait();
      break;
    case DESTINY:
      print2("The Lords of Destiny ignore your lack of faith.");
      sacrilege = FALSE;
      morewait();
      break;
    case DRUID:
      print2("Your treachery to the ArchDruid has been noted.");
      if (random_range(2) == 1)
	Player.alignment += 40;
      else Player.alignment -= 40;
      morewait();
      break;
    }
    if (sacrilege) {
      Player.patron = 0;
      Player.rank[PRIESTHOOD] = 0;
    }
  }
  return(sacrilege);
}

int increase_priest_rank(int deity)
{
  if (Player.rank[PRIESTHOOD] == 0) switch(deity) {
  default:
    print2("Jokin nimetön jumala siunaa sinut...");
    Player.hp = max(Player.hp, Player.maxhp);
    morewait();
    print2("Alttari hajoaa pölyksi ja lentää pois.");
    Level->site[Player.x][Player.y].locchar = FLOOR;
    Level->site[Player.x][Player.y].p_locf = L_NO_OP;
    lset(Player.x, Player.y, CHANGED);
    break;
  case ODIN:
    if (Player.alignment > 0) {
      print1("Ukko Ylijumala hears your prayer! Ukko Ylijumala kuulee rukouksesi!");
      print2(Priest[ODIN]);
      nprint2(" henkilökohtaisesti siunaa sinut.");
      nprint2(" Olet nyt Ukko Ylijumalan maallikkopalvoja.");
      Player.patron = ODIN;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      /* DAG learnclericalspells() starts with mprint() so don't need morewait */
      /* morewait(); */
      learnclericalspells(ODIN,LAY);
    }
    else print1("Ukko Ylijumala jättää sinut huomiotta.");
    break;
  case SET:
    if (Player.alignment < 0) {
      print1("Perkele hears your prayer!");
      print2(Priest[SET]);
      nprint2(" personally blesses you. ");
      nprint2(" You are now a lay devotee of Perkele.");
      Player.patron = SET;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      /* DAG learnclericalspells() starts with mprint() so don't need morewait */
      /* morewait(); */
      learnclericalspells(SET,LAY);
    }
    else print1("Perkele jättää sinut huomiotta.");
    break;
  case ATHENA:
    if (Player.alignment > 0) {
      print1("Mielikki kuulee rukouksesi!");
      print2(Priest[ATHENA]);
      nprint2(" henkilökohtaisesti siunaa sinut.");
      nprint2(" Olet nyt Mielikin maallikkopalvoja.");
      Player.patron = ATHENA;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      /* DAG learnclericalspells() starts with mprint() so don't need morewait */
      /* morewait(); */
      learnclericalspells(ATHENA,LAY);
    }
    else print1("Mielikki jättää sinut huomiotta.");
    break;
  case HECATE:
    if (Player.alignment < 0) {
      print1("Louhi kuulee rukouksesi!");
      print2(Priest[HECATE]);
      nprint2(" henkilökohtaisesti siunaa sinut.");
      nprint2(" Olet nyt Louhen maallikkopalvoja.");
      Player.patron = HECATE;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      /* DAG learnclericalspells() starts with mprint() so don't need morewait */
      /* morewait(); */
      learnclericalspells(HECATE,LAY);
    }
    else print1("Louhi jättää sinut huomiotta.");
    break;
  case DRUID:
    if (abs(Player.alignment) < 10) {
      print1(Priest[DRUID]);
      nprint1(" henkilökohtaisesti sinuaa sinut.");
      print2("Olet nyt Tietäjien maallikkojäsen.");
      Player.patron = DRUID;
      Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
      /* DAG learnclericalspells() starts with mprint() so don't need morewait */
      /* morewait(); */
      learnclericalspells(DRUID,LAY);
    }
    else {
      print1("You hear a voice....");
      morewait();
      print2("'Only those who embody the Balance may become Druids.'");
    }
    break;
  case DESTINY:
    print1("The Lords of Destiny could hardly care less.");
    print2("But you can consider yourself now to be a lay devotee.");
    Player.patron = DESTINY;
    Player.rank[PRIESTHOOD] = LAY;
      Player.guildxp[PRIESTHOOD] = 1;
    break;
  }
  else if (deity == Player.patron) {
    if ((((deity == ODIN) || (deity == ATHENA)) && 
	 (Player.alignment < 1)) ||
	(((deity == SET) || (deity == HECATE)) && 
	 (Player.alignment > 1)) ||
	((deity == DRUID) && (abs(Player.alignment) > 10))) {
      print1("You have swerved from the One True Path!");
      print2("Your deity is greatly displeased...");
      Player.xp -= Player.level*Player.level;
      Player.xp = max(0,Player.xp);
    }
    else if (Player.rank[PRIESTHOOD]== HIGHPRIEST) return 0;
    else if (Player.rank[PRIESTHOOD]== SPRIEST) {
      if (Player.level > Priestlevel[deity])
	hp_req_test();
      else return 0;
    }
    else if (Player.rank[PRIESTHOOD]==PRIEST) {
      if (Player.guildxp[PRIESTHOOD] >= 4000) {
	print1("An heavenly fanfare surrounds you!");
	print2("Your deity raises you to the post of Senior Priest.");
	hp_req_print();
	Player.rank[PRIESTHOOD] = SPRIEST;
        /* DAG learnclericalspells() starts with mprint() so don't need morewait */
	/* morewait(); */
	learnclericalspells(deity,SPRIEST);
      }
      else return 0;
    }
    else if (Player.rank[PRIESTHOOD]==ACOLYTE) {
      if (Player.guildxp[PRIESTHOOD] >= 1500) {
	print1("A trumpet sounds in the distance.");
	print2("Your deity raises you to the post of Priest.");
	Player.rank[PRIESTHOOD] = PRIEST;
        /* DAG learnclericalspells() starts with mprint() so don't need morewait */
	/* morewait(); */
	learnclericalspells(deity,PRIEST);
      }
      else return 0;
    }
    else if (Player.rank[PRIESTHOOD]==LAY) {
      if (Player.guildxp[PRIESTHOOD] >= 400) {
	print1("A mellifluous chime sounds from above the altar.");
	print2("Your deity raises you to the post of Acolyte.");
	Player.rank[PRIESTHOOD] = ACOLYTE;
        /* DAG learnclericalspells() starts with mprint() so don't need morewait */
	/* morewait(); */
	learnclericalspells(deity,ACOLYTE);
      }
      else return 0;
    }
  }
  return 1;
}


void answer_prayer(void)
{
  clearmsg();
  switch(random_range(12)) {
  case 0: print1("You have a revelation!"); break;
  case 1: print1("You feel pious."); break;
  case 2: print1("A feeling of sanctity comes over you."); break;
  default: print1("Nothing unusual seems to happen."); break;
  }
}


void hp_req_test(void)
{
  pob o;
  switch  (Player.patron) {
  case ODIN:
    if (find_item(&o,OB_SYMBOL_SET,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case SET:
    if (find_item(&o,OB_SYMBOL_ODIN,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case ATHENA:
    if (find_item(&o,OB_SYMBOL_HECATE,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case HECATE:
    if (find_item(&o,OB_SYMBOL_ATHENA,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case DRUID:
    if (find_item(&o,OB_SYMBOL_ODIN,-1))
      make_hp(o);
    else if (find_item(&o,OB_SYMBOL_SET,-1))
      make_hp(o);
    else if (find_item(&o,OB_SYMBOL_ATHENA,-1))
      make_hp(o);
    else if (find_item(&o,OB_SYMBOL_HECATE,-1))
      make_hp(o);
    else hp_req_print();
    break;
  case DESTINY:
    if (find_item(&o,OB_SYMBOL_DESTINY,-1))
      make_hp(o);
    else hp_req_print();
    break;
  }
}


void hp_req_print(void)
{
  morewait();
  print1("To advance further, you must obtain the Holy Symbol of ");
  switch(Player.patron) {
  case ODIN:
    nprint1(Priest[SET]);
    print2("kuka löytyy Perkeleen päätemppelistä.");
    break;
  case SET:
    nprint1(Priest[ODIN]);
    print2("kuka löytyy Ukko Ylijumalan päätemppelistä.");
    break;
  case ATHENA:
    nprint1(Priest[HECATE]);
    print2("kuka löytyy Louhin päätemppelistä.");
    break;
  case HECATE:
    nprint1(Priest[ATHENA]);
    print2("kuka löytyy Mielikin päätemppelistä.");
    break;
  case DRUID:
    print2("kuka tahansa oikein suuntautunut pappi temppeleistään.");
    break;
  case DESTINY:
    nprint1(Priest[DESTINY]);
    print2("kuka löytyy Kohtalon päätemppelistä.");
    break;
  }
}

void make_hp(pob o)
{
  print1("A full-scale heavenly choir chants 'Hallelujah' all around you!");
  print2("You notice a change in the symbol you carry....");
  switch(Player.patron) {
  case ODIN:
    *o = Objects[OB_SYMBOL_ODIN];
    break;
  case SET:
    *o = Objects[OB_SYMBOL_SET];
    break;
  case ATHENA:
    *o = Objects[OB_SYMBOL_ATHENA];
    break;
  case HECATE:
    *o = Objects[OB_SYMBOL_HECATE];
    break;
  case DRUID:
    *o = Objects[OB_SYMBOL_DRUID];
    break;
  case DESTINY:
    *o = Objects[OB_SYMBOL_DESTINY];
    break;
  }
  o->known = 2;
  o->charge = 17; /* random hack to convey bit that symbol is functional */
  morewait();
  if (Player.patron == DRUID)
    print1("Your deity raises you to the post of ArchDruid!");
  else print1("Your deity raises you to the post of High Priest!");
  print2("You feel holy.");
  strcpy(Priest[Player.patron],Player.name);
  Priestlevel[Player.patron] = Player.level;
  Player.rank[PRIESTHOOD] = HIGHPRIEST;
  morewait();
  Priestbehavior[Player.patron] = fixnpc(4);
  save_hiscore_npc(Player.patron);
  learnclericalspells(Player.patron,HIGHPRIEST);
}

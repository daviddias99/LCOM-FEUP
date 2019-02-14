PROG=proj

SRCS = proj.c game.c keyboard.c timer.c mouse.c videocard.c videocard_util.c rtc.c rtc_ih.S GameStateMachine.c Cursor.c Button.c Sprite.c AnimatedSprite.c Enemy.c Projectile.c  highscores.c linked_list.c dl_math.c wordWriter.c serialport.c

CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>

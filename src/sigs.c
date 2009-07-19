/*	$calcurse: sigs.c,v 1.9 2009/07/19 16:51:36 culot Exp $	*/

/*
 * Calcurse - text-based organizer
 *
 * Copyright (c) 2007-2009 Frederic Culot <frederic@culot.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the
 *        following disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the
 *        following disclaimer in the documentation and/or other
 *        materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Send your feedback or comments to : calcurse@culot.org
 * Calcurse home page : http://culot.org/calcurse
 *
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "i18n.h"
#include "utils.h"
#include "sigs.h"

/* 
 * General signal handling routine.
 * Catch return values from children (user-defined notification commands).
 * This is needed to avoid zombie processes running on system.
 * Also catch CTRL-C (SIGINT), and SIGWINCH to resize screen automatically.
 */
static void
generic_hdlr (int sig)
{
  switch (sig)
    {
    case SIGCHLD:
      while (waitpid (WAIT_MYPGRP, NULL, WNOHANG) > 0)
	;
      break;
    case SIGWINCH:
      clearok (curscr, TRUE);
      (void)ungetch (KEY_RESIZE);
      break;
    }
}

unsigned
sigs_set_hdlr (int sig, void (*handler)(int))
{
  struct sigaction sa;

  memset (&sa, 0, sizeof sa);
  sigemptyset (&sa.sa_mask);
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  if (sigaction (sig, &sa, (struct sigaction *)0) == -1)
    {
      ERROR_MSG (_("Error setting signal #%d : %s\n"),
                 sig, strerror (errno));
      return 0;
    }

  return 1;
}

/* Signal handling init. */
void
sigs_init ()
{
  if (!sigs_set_hdlr (SIGCHLD, generic_hdlr)
      || !sigs_set_hdlr (SIGWINCH, generic_hdlr)
      || !sigs_set_hdlr (SIGINT, SIG_IGN))
    exit_calcurse (1);
}

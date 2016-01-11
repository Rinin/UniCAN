#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "unican.h"

#include "linux/can.h"
#include "lib.h"

#define BUFSZ 520

int verbose = 0;
int unican_errors = 0;
int unican_messages = 0;

/**INDENT-OFF**/
static void usage(const char *prg)
{
	printf("Usage: %s [options]\n", prg);
	printf("\n");
	printf("Options:\n");
	printf("  -v\t\t(increase verbose level)\n");
	printf("\n");

	exit(1);
}
/**INDENT-ON**/

void can_HW_init(void)
{
	return;
}

void can_HW_close(void)
{
	if (verbose) {
		printf("\n");
		printf("UNICAN summary\n\n");
		printf("UNICAN error(s): %d\n", unican_errors);
		printf("UNICAN message(s): %d\n", unican_messages);
		printf("\n");
	} else
		printf("%d\t%d\n", unican_errors, unican_messages);

	return;
}

void can_HW_receive_message(void)
{
	return;
}

void can_HW_send_message(can_message * msg)
{
	return;
}

void unican_error(uint16_t errcode)
{
	static const char *z[18] = {
		/*  0 */ "UNICAN_OK",
		/*  1 */ "UNICAN_OFFLINE",
		/*  2 */ "UNICAN_WARNING_BUFFER_OVERWRITE",
		/*  3 */ "UNICAN_WRONG_CRC",
		/*  4 */ "UNICAN_NO_FREE_BUFFER",
		/*  5 */ "UNICAN_DATA_WITHOUT_START",
		/*  6 */ "UNICAN_WARNING_UNEXPECTED_DATA",
		/*  7 */ "UNICAN_UNKNOWN_ERROR",
		/*  8 */ "UNICAN_UNKNOWN_ERROR",
		/*  9 */ "UNICAN_UNKNOWN_ERROR",
		/* 10 */ "UNICAN_UNKNOWN_ERROR",
		/* 11 */ "UNICAN_CAN_MESSAGE_TOO_SHORT",
		/* 12 */ "UNICAN_CAN_MESSAGE_TOO_LONG",
		/* 13 */ "UNICAN_CAN_IDENTIFIER_TOO_LONG",
		/* 14 */ "UNICAN_CAN_IDENTIFIER_EXT_TOO_LONG",
		/* 15 */ "UNICAN_HEADER_TOO_SHORT",
		/* 16 */ "UNICAN_CANT_ALLOCATE_NODE",
		/* 17 */ "UNICAN_HW_ERROR"
	};

	unican_errors++;

	fprintf(stderr, "Error %d <%s>\n", errcode, &z[errcode][7]);
}

void unican_RX_event(uint16_t msg_id, uint16_t length)
{
	if (verbose)
		printf("Event\tmid=%04x len=%d\n", msg_id, length);
	/*
	 * WARNING. This is a bad place for unican_take_message().
	 */
	unican_take_message();
}

void unican_RX_message(unican_message * msg)
{
	unican_messages++;
	if (verbose)
		printf("Message\tmid=%04x len=%d buf=%p\n", msg->unican_msg_id,
		       msg->unican_length, msg->data);
}

int main(int argc, char *argv[])
{
	int eof;
	int mtu;
	int opt;
	long sec, usec;
	char *fret;
	FILE *infile = stdin;
	can_message message;
	struct canfd_frame frame;
	static char buf[BUFSZ], device[BUFSZ], ascframe[BUFSZ];

	while ((opt = getopt(argc, argv, "v")) != -1) {
		switch (opt) {
		case 'v':
			verbose++;
			break;
		default:
			usage(argv[0]);
			break;
		}
	}

	unican_init();

	/* read first non-comment frame from input */
	while ((fret = fgets(buf, BUFSZ - 1, infile)) != NULL && buf[0] != '(') {
		if (strlen(buf) >= BUFSZ - 2) {
			fprintf(stderr,
				"comment line too long for input buffer\n");
			return 1;
		}
	}

	if (!fret)
		goto out;	/* nothing to read */

	eof = 0;

	if (sscanf(buf, "(%ld.%ld) %s %s", &sec, &usec, device, ascframe) != 4) {
		fprintf(stderr, "incorrect line format in logfile\n");
		return 1;
	}

	while (!eof) {

		mtu = parse_canframe(ascframe, &frame);
		if (!mtu) {
			fprintf(stderr, "wrong CAN frame format: '%s'!\n",
				ascframe);
			return 1;
		} else {
			memset(&message, 0, sizeof(message));
			if (frame.can_id & CAN_EFF_FLAG) {
				message.can_identifier =
				    frame.can_id & CAN_EFF_MASK;
				message.can_extbit = 1;
			} else {
				message.can_identifier =
				    frame.can_id & CAN_SFF_MASK;
				message.can_extbit = 0;
			}
			if (frame.can_id & CAN_RTR_FLAG) {
				message.can_rtr = 1;
				message.can_dlc = 0;
			} else {
				int n;
				message.can_rtr = 0;
				message.can_dlc = frame.len;
				n = (frame.len + 7) / 8;
				switch (frame.len % 8) {
				case 0:
					do {
						message.data[0] = frame.data[0];
				case 7:
						message.data[7] = frame.data[7];
				case 6:
						message.data[6] = frame.data[6];
				case 5:
						message.data[5] = frame.data[5];
				case 4:
						message.data[4] = frame.data[4];
				case 3:
						message.data[3] = frame.data[3];
				case 2:
						message.data[2] = frame.data[2];
				case 1:
						message.data[1] = frame.data[1];
					} while (n-- > 0);
				}
			}
			/* NOTICE: can_HW_receive_message() should call it */
			can_receive_message(&message);
		}

		/* read next non-comment frame from input */
		while ((fret = fgets(buf, BUFSZ - 1, infile)) != NULL
		       && buf[0] != '(') {
			if (strlen(buf) >= BUFSZ - 2) {
				fprintf(stderr,
					"comment line too long for input buffer\n");
				return 1;
			}
		}

		if (!fret) {
			eof = 1;	/* this file is completely processed */
			break;
		}

		if (sscanf(buf, "(%ld.%ld) %s %s", &sec, &usec,
			   device, ascframe) != 4) {
			fprintf(stderr, "incorrect line format in logfile\n");
			return 1;
		}

	}			/* while (!eof) */

out:
	fclose(infile);
	unican_close();

	return 0;
}

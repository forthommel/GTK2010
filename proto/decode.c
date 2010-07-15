#include <stdio.h>
#include <stdlib.h>
/* Output buffer := {global header, event := {TDC Header, TDC Measurements, TDC Error, TDC Trailer}, ETTT, Trailer} */


/* Word type */
typedef enum {
	global_header = 0x8,	
	tdc_header = 0x1,
	tdc_measur = 0x0,
	tdc_trailer = 0x3,
	tdc_error = 0x4,
	ettt = 0x11,
	global_trailer = 0x10,
	filler = 0x18, 
} word_type;

/* Hit structure (N in each event) */
typedef struct {
	int tdc; /* TDC identification */
	int trailead; /* Trailing or leading measurement */
	int channel; /* Channel */
	int hit_id; /* ? */
	int bunch_id; /* ? */
	int tdc_measur; /* */
	int word_count; /* ? */
	int error_flags; /* ? */
} hit_t;

/* Event structure (one for each trigger) */
typedef struct {
	int geo; /* GEO address */
	int ettt; /* Extended trigger time tag */
	int status; /* TDC error, output buffer overflow, trigger lost */
	int word_count; /* ? */
	int cur_pos; /* Current position in the hits memory zone */
	hit_t * hits;
	int nb_hits;
} event_t;


/* Declarations */
void fill(int word, event_t *blk ); /* FIXME rename me*/

int main(int argc, char *argv[]) {
  //dummy[0] -> global_header (event = 42, geo = 1)
  //dummy[1] -> tdc_measurement [single edge] (trailing measurement, channel = 0, trailing time = 10) 
  int i;
  event_t evt; /* FIXME initilalize the structure */
  int blk_size = 3;
  int dummy[3] = {0x40000541,0x400000A,0x400000A};  
  /* First word of the block (!!! assuming Event Aligned BLT !!!) */ 
  if((dummy[0] >> 27) != global_header) {
    /* Abort */
    printf("First word is not the global header, abort\n");
  }
  /* Read the hits number */
  evt.nb_hits = (dummy[0]&0x7FFFFE0) >> 5;
  printf("%d hits in the event (trigger)\n",evt.nb_hits);
  /* Reserve memory for the hits FIXME reserve only one time, one big chunk of memory */
  evt.hits = (hit_t*) calloc(evt.nb_hits,sizeof(hit_t));
  evt.cur_pos = 0; /* move the cursor */	
  /* read the hits and fill the structure */
  for(i = 1;i < blk_size-1;i++) {
    fill(dummy[i],&evt);
  }
  /* Last word must be global_trailer */
  if((dummy[i] >> 27) != global_trailer) {
    /* Abort */
    printf("Last word is not the global trailer, abort\n");
  }

  /* --------*/

  /* Display the block */
  for(i = 0; i < evt.nb_hits; i++) {
    printf("Channel: %d, Measurement: %d\n",evt.hits[i].channel,evt.hits[i].tdc_measur);
  }
  /* Not forget to free the memory !*/
  free(evt.hits);
  return 0;
}

/* Fill the block structure */ 
/*
	!!! Assuming TDC header, TDC error and TDC trailer are enabled !!!
	If it is not the case cur_pos incrementation must be moved. (FIXME ?)
*/
void fill(int word, event_t *evt) {
	int id_word = word >> 27;
	switch(id_word) {
		case global_header: 
			/* Second global_header ?! Abort */
			printf("Second header, abort\n");
			break;
		case tdc_header:
			/* FIXME masking and shifting */
			evt->hits[evt->cur_pos].hit_id = 0;
			evt->hits[evt->cur_pos].bunch_id = 0;
			break;
		case tdc_measur:
			evt->hits[evt->cur_pos].tdc_measur = word&0x7ffff; 
			evt->hits[evt->cur_pos].channel = (word&0x3f8000) >> 19;
			evt->hits[evt->cur_pos].trailead = (word&0x4000000) >> 26;
			break;
		case tdc_trailer:
			if(evt->cur_pos >= (evt->nb_hits - 1)) {
				/* Too many hits ! Abort */
				printf("Announced hits nb != actual hits nb, abort\n");
				break;
			}
			evt->hits[evt->cur_pos].word_count = 0;
			evt->cur_pos++; /* Last item of the hit, move to the next one */	
			break;
		case tdc_error:
			break;
		case ettt:
			break;
		case global_trailer:
			/* Trailer not in the last position, abort */
			printf("Trailer not in the last position\n");
			break;
		case filler:
			break;
		default:
			printf("Unknown word, abort\n");
		//Unknown word
	}
}


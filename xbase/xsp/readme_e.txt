==========================================================================
                     XSP Sprite Management System
                           ver 2.02 by Yosshin
                  Translation ver 1.00 by Mike Moffitt
==========================================================================

~~~~~~~~~~~~~~~~~~~~~~~~~~ Translator Notes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

I've elected to translate this documentation in the ripe year
of 2022, well after the release of this library. This doc and the code
it describes are nearly old enough to drink, so please note that software
engineering conventions may have changed a little since it was written.

I've added an explicit (void) to functions that take no parameters. This
has always been valid and will not require changes to code. I have only
done it out of nitpicky "correctness".

Otherwise, I have attempted to make the most direct translation that I am
able to do. There are one or two points where I've added a (TL Note) to
clarify something that uses terms that could be unclear to a native
English speaker. For something like this, I would rather provide a note
for clarification than massively overhaul a section.

==========================================================================
                              Explanation
==========================================================================

XSP is a sprite management system available for the Sharp X680x0 series.
Its functions are as follows.

1) With a method called "Sprite Doubler", up to 512 sprites can be displayed
(four times what is provided by the hardware).

2) Although the X680x0's PCG area only has space for 256 patterns, up to a
maximum of 32768 patterns may be used. With dynamic PCG memory allocation,
the following optimizations are done.

  * Pattern data already in PCG is re-used (a cache hit).
  
  * If PCG memory is unavailable for new pattern data, a region of already used
    PCG is overwritten.

  * PCG memory is kept as long as possible, as it may be needed later.
  
3) It is no longer to specify sprite number as with the X-BASIC sp_set function.

4) Sprite priority may be specified in 64 steps (in reality, 48 steps).

5) Multiple sprites may be combined to form a large sprite (composite sprite).
   The calling overhead is reduced when compared to manually displaying each
sprite individually. The arrangement may be freely defined by the user.

6) A sprite set function is available that allows for the passage of parameters
   via a struct pointer. It can reduce calling overhead when being called from
   the C language (which would otherwise place four arguments onto the stack).

7) Triple buffering is supported. As a result, it is possible to isolate the
   game loop's timings from that of vertical synchronization. A judgement can be
   made automatically to skip one frame of display processing and allow an early
   return from the vertical sync wait function to allow the game to catch up.

   When the processing time of the main loop becomes intense, and 60fps can no
   longer be achieved, automatic disregard for the vertical sync allows for it
   to drop up to 30fps without slowing the game rate. Plus, by skipping one
   frame of display requests, the game loop may advance by one frame compared to
   double-buffering, which would expect a stabilization of the framerate.
   (This 1-frame advance execution results in a 1-frame display latency penalty,
   so it is up to the user to enable it or not).
   
8) The user may also use vertical blank and raster interrupts without a conflict
   with XSP (Though, attaching the interrupt request handlers must be done using
   dedicated functions provided by XSP).

9) In order to allow XSP to work alongside PCM8A.X (created by philly), a
   function is provided to configure the interrupt mask for PCM8A.X.
   
10) It's been optimized to the fullest extent possible.

What follows are notes and limitations.

1) When displaying 129 sprites or greater, the sprite priority may be incorrect
   on the raster interrupt seam (Functionality to reduce this priority error has
   been implemented, but it is imperfect).

2) It is difficult to use PCG for both BG and sprites (it may be possible).

3) Only the 256x256 pixel screen size is supported.



==========================================================================
                      Summary of Steps to Use XSP
==========================================================================

Here is a summary of XSP's usage. For greater details, please see the
notes pertaining to each function or the sample code.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Initialization ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Step 1) * Set the screen size to 256x256 pixels.

        * Enable sprite display.
        
        * Enable XSP by calling xsp_on.

Step 2) * Pass PCG data and PCG placement work memory pointers by calling
          the xsp_pcgdat_set function.
          
        * If it will be used, associate composite sprite mapping data by
          calling the xsp_objdat_set function.
          
        * If needed, vertical mode may be set by calling xsp_vertical.
          XSP also offers a function to rotate all PCG pattern data by 90
          degrees. Do so by calling pcg_roll90.
          
          
~~~~~~~~~~~~~~~ Usage During the Main Loop of the Game ~~~~~~~~~~~~~~~~~~~

Step 3) * If appropriate, call xsp_vsync for vertical synchronization.

Step 4) * Call xsp_set, xobj_set, etc. to batch sprites for display.

Step 5) * Call xsp_out to render the previously batched sprites.

     (Return to step 3.)


~~~~~~~~~~~~~~~~~ Usage During Termination of the Program ~~~~~~~~~~~~~~~~

Step 6) * Call xsp_off to disable XSP.


~~~~~~~~~~~~~~~~~~~~~~~~~~ Additional Notes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Note 1) If the PCG tile data has changed (for example, by loading new
        sprite data), it must be passed to XSP by calling xsp_pcgdat_set.
          
Note 2) If needed, call the xsp_pcgmask_on/off functions to allocate and
        release areas of PCG used by XSP. (This is always available after
        xsp_on has been called).

Note 3) You may optionally call xsp_mode to set the algorithm used to
        multiplex sprites. (This may be done after xsp_on has been called).
        
Note 4) You may optionally call xsp_vsyncint_on/off to set or clear the
        vertical blank interrupt process. (This may be done after xsp_on).
        
Note 5) Raster line interrupts may be set or cleared using the
        xsp_hsyncint_on/off functions. (May be done after calling xsp_on).


==========================================================================
                     Function Calling Convention
==========================================================================

All XSP functions follow the C calling convention. If you are calling XSP
functions from assembly code, please follow the C calling convention.

This section explains how to call C functions from assembly.

Step 1) Save registers d0-d2/a0-a2 to the stack. (Even though it is not
        guaraneteed that a function will clobber any or all of those
        registers, they should be guarded against future changes).
        
        * TL Note: If your code will not be bothered by those registers
        being changed from the function call, then you may skip this.

Step 2) Function arguments are pushed to the stack, starting from the last
        parameter. All arguments are aligned to long word (32-bit) size,
        regardless of the type of the argument.
        
Step 3) Call the function using the bsr or jsr instructions. The label
        to jump to is the C function name, prefixed with an underscore.
        
        The return value is placed in d0, and the type of the data depends
        upon the function type. (For example, a function returning a short
        / int16_t will only place valid data in the lower 16 bits.)
        
Step 4) Align the stack in accordance to the amount offset from step 2.

Step 5) Restore the saved registers d0-d2/a0-a2 from the first step.

Below is a real example of calling a C function from assembly.

	movem.l	d0-d2/a0-a2,-(sp) * Register save
	move.l	#$013F,-(sp)      * INFO
	move.l	#0,-(sp)          * PT
	move.l	#200,-(sp)        * Y
	move.l	#100,-(sp)        * X
	jsr	_xsp_set              * Function call (don't forget the underbar!)
	                          * d0.w contains the return value.
	lea	4*4(sp),sp            * Stack adjustment (four arguments)
	movem.l	(sp)+,d0-d2/a0-a2 * Register restore
	
This example is the equivalent of calling xsp_set(100, 200, 0, 0x13F).


==========================================================================
                          Function Explanation
==========================================================================

● XSP Initialization

Prototype:   void xsp_on(void);

Return:      None

Description: Initializes XSP. This must be called at the beginning.

             Note that if any vertical blank or raster interrupts were
             registered before, they will be ignored. 

--------------------------------------------------------------------------

● XSP Termination

Prototype:   void xsp_off(void);

Return:      None

Description: Terminates XSP. At the time of program shutdown, always call
             this function. Otherwise, there will certainly be execution
             issues. All interrupt vectors and other state are restored to
             how they were prior to XSP initialization.

--------------------------------------------------------------------------

● PCG data and PCG placement work memory setup

Prototype:   void xsp_pcgdat_set(const void *pcg_dat, char *pcg_alt,
                                 short alt_size);
                                 
Arguments:   void  *pcg_dat : PCG data pointer
             char  *pcg_alt : PCG placement work memory pointer
             short alt_size : PCG placement work memory size (byte count)

Return:      None

Description: Assigns PCG data and PCG placement work memory to XSP. This must
             be called after xsp_on is called. If the PCG data itself has
             changed (for example, after loading new sprite data, etc) do be
             sure to re-specify the data by calling this function again.
             
             The PCG placement work memory is used internally by XSP. It is
             automatically initialized (cleared to zero) by this function, so
             the user does not need to initialize it manually.
             
             The PCG data format is the "beta" format (described later). PCG
             data (with the extension ".SP") exported by the sprite tool
             SM.X (From the June 1992 issue of Oh! X) or EEL.X (CATsoft/GORRY)
             may be used as-is.
             
             TL Note: The following paragraph was a little dated with its advice
             so I have made adjustments to let it be more specific.
             
             PCG data must lie on an address with even alignment. If using an
             assembler, use a .align 2 directive or something similar. For C,
             with GCC, you can create an int16_t array, or simply use
             __attribute__((aligned(2))) when creating the buffer. 
             
             A PCG sprite is divided into four tiles, each 8x8 pixels. 


                                    ↑  ┌──────┬──────┬──────┬──────┐
                                    │  │ +00  │ +01  │ +02  │ +03  │
                                    │  ├──────┼──────┼──────┼──────┤
                                    │  │ +04  │ +05  │ +06  │ +07  │
          Tile 0 enlarged view:     │  ├──────┼──────┼──────┼──────┤
                ┌────────────→      │  │ +08  │ +09  │ +0A  │ +0B  │
                │                   │  ├──────┼──────┼──────┼──────┤
                │                      │ +0C  │ +0D  │ +0E  │ +0F  │
      ↑    ┏━━━━━━┳━━━━━━┓   8 pixels  ├──────┼──────┼──────┼──────┤
      │    ┃      ┃      ┃             │ +10  │ +11  │ +12  │ +13  │
      |    ┃Tile 0┃Tile 1┃          │  ├──────┼──────┼──────┼──────┤
           ┃      ┃      ┃          │  │ +14  │ +15  │ +16  │ +17  │
16 pixels  ┣━━━━━━╋━━━━━━┫          │  ├──────┼──────┼──────┼──────┤
           ┃      ┃      ┃          │  │ +18  │ +19  │ +1A  │ +1B  │
      │    ┃Tile 2┃Tile 3┃          │  ├──────┼──────┼──────┼──────┤
      │    ┃      ┃      ┃          │  │ +1C  │ +1D  │ +1E  │ +1F  │
      ↓    ┗━━━━━━┻━━━━━━┛          ↓  └──────┴──────┴──────┴──────┘

           ←──16 pixels──→              ←──────── 8 pixels ────────→
           
             Each number in this figure represents the offset (in bytes) from
             the beginning of the data, in hexidecimal radix. Every square
             above is one byte, where the upper and lower nybbles represent
             a color code (16 colors). In other words, one byte is two pixels,
             resulting in 32 bytes per 8x8 tile. The same is true of the other
             three tiles. All four tiles consecutively make one 16x16 PCG tile.
             One 16x16 PCG tile is thus 128 bytes.
             
             TL Note: The format is simply 4bpp linear arrangement, with four
             8x8 tiles in a row-interleaved order forming a 16x16 bitmap, per
             sprite. This is identical to the way Megadrive games store a 2x2
             cell sprite in VRAM.
             
             
             PCG tiles like this are placed in memory, in sequence, to form the
             block of PCG data (as shown below).

          +0x000┌──────────────┐
                │              │
                │Pattern No. 0 │
                │              │
          +0x080├──────────────┤
                │              │
                │Pattern No. 1 │
                │              │
          +0x100├──────────────┤
                │              │
                │Pattern No. 2 │
                │              │
          +0x180├──────────────┤
                   ：                ：


--------------------------------------------------------------------------

● Masking an area of PCG for XSP to leave unmanaged

Prototype:   void xsp_pcgmask_on(short start_no, short end_no);
                                 
Arguments:   short start_no : Mask start PCG code number
             short end_no   : Mask end PCG code number

Return:      None

Description: Specifies an area of PCG to mask off for XSP to not use. Areas
             masked off will not be modified by XSP.
             
             For example, if the user is already using part of PCG for
             background tiles, or is using part of PCG RAM for background
             layout attributes (PCG codes 128 - 255), it must be masked off
             so that XSP does not clobber it.

--------------------------------------------------------------------------

● Unmasking an area of PCG for XSP to leave unmanaged

Prototype:   void xsp_pcgmask_off(short start_no, short end_no);
                                 
Arguments:   short start_no : Mask start PCG code number
             short end_no   : Mask end PCG code number

Return:      None

Description: Cancels the setting of a masked area of PCG memory. Please see
             xsp_pcgmask_on's description for details.

--------------------------------------------------------------------------

● Selection of sprite multiplication algorithm

Prototype:   void xsp_mode(short mode_no);
                                 
Arguments:   short mode_no : 1 = Flicker multiplexing of 128 sprites (max 384)
                      2 = Maximum of 512 sprites (default)
                      3 = Maximum of 512 sprites, priority error mitigation

Return:      None

Description: Specifies sprite multiplication algorithm.

     Mode 1: Through the use of 128 hardware sprites, up to 384 sprites are
             displayed by flickering sprites between positions. Sprite flicker
             is done in a way that gives high priority sprites preference.
             
     Mode 2: In this mode, up to 512 sprites are drawn through the use of
             raster splits. For the sake of simplicity for the algorithm,
             there may be tears in sprites placed on the raster split "seams".
             
     Mode 3: Similar to Mode 2, but techniques are employed to reduce the
             collapse of the priority system across the raster splits. 
             If the draw count is very high (in excess of 300, roughly) the
             mitigation techniques may fail, and the priority error may occur.
             The processing load of this mode is a little higher than Mode 2.

--------------------------------------------------------------------------

● Specification of composite sprite mapping data

Prototype:   void xsp_objdat_set(const void *sp_ref);
                                 
Arguments:   void *sp_ref : Composite sprite reference data pointer

Return:      None

Description: Associates composite sprite mapping data with XSP. Execute this
             function before using composite sprites.

--------------------------------------------------------------------------

● Wait for vertical sync

Prototype:   short xsp_vsync(short n);
                                 
Arguments:   short n : Number of vertical blank intervals to wait

Return:      Number of missed vertical blank periods.
             -1 if XSP was not included (TL note: Included? What does
             kumikomarete inakattanara mean in this case?
             XSP wasn't initialized?)

Description: Waits for vertical vsync for the specified number of periods.
             It has been designed to skip one vblank wait if processing delay
             has occured (to allow the game to catch up). Use this in the 
             main loop of the game.
             
             If 0 is specified for n, one frame of advance processing will be
             performed. If this is done, the output of the game can be smoothed
             in the event of processing drop-out, but it incurs a frame of
             display lag.

--------------------------------------------------------------------------

● Registering sprites for display

Prototype:   short xsp_set(short x, short y, short pt, short info);
                                 
Arguments:   short x    : Sprite X position
             short y    : Sprite Y position
             short pt   : Sprite PCG pattern no.（0-0x7FFF）
             short info : Data for reversal, color, display priority

Return:      If the sprite's coordinates (x,y) were off-screen, 0 is returned.
             Otherwise, a non-zero value is returned.

Description: This function registers a sprite for display. All registered
             sprites are displayed at once upon calling xsp_out.
             
             The "info" argument is a bitfield indicating sprite reversal,
             palette number, and display priority.

      info = 0x****
               ↑↑↑↑
               ││││
               ││└┴────────  Priority表示優先度  （0-0x3F）
               ││
               │└──────────  Color Code (palette line)（0-0xF）
               │
               └───────────  Reversal code  0x0：Normal
                                            0x4：Horizontal flip
                                            0x8：Vertical flip
                                            0xC：Both
                                            
             Higher display priority numbers are displayed on top. The second
             digit of the priority level represents interaction with the BG
             planes. Below is the specification.

             Priority 0x30-0x3F : Above BG0
             Priority 0x20-0x2F : Between BG0 and BG1
             Priority 0x10-0x1F : Below BG1
             Priority 0x00-0x0F : Not displayed


--------------------------------------------------------------------------

● Registering composite sprites for display

Prototype:   void xobj_set(short x, short y, short pt, short info);
                                 
Arguments:   short x    : Composite sprite X position
             short y    : Composite sprite Y position
             short pt   : Composite sprite mapping (REF) no.（0-0x0FFF）
             short info : Data for reversal, color, display priority

Return:      None

Description: Performs a registration of a composite sprite for display.
             Registered sprites are displayed in one batch upon calling xsp_out.
             
             Please read the description for xsp_set for an explanation of the
             `info` argument.
             
             The list of composite sprites (format expanded upon later) must
             have already been prepared by the user prior to calling xobj_set.
             This is done using xsp_objdat_set. Behavior is not predictable if
             xsp_objdat_set is called without having associated composite sprite
             mapping data first. 

--------------------------------------------------------------------------

● Registering sprites for display (with struct pointer)

Prototype:   short xsp_set_st(const void *arg);
                                 
Arguments:   void *arg : Parameter struct pointer

Return:      If the sprite's coordinates (x,y) were off-screen, 0 is returned.
             Otherwise, a non-zero value is returned.

Description: This function is a little faster than the xsp_set function. Please
             read the description of xsp_set for more details.
             
             The struct format is as follows:
             
        ┌────────── Offset from the start of the struct (in bytes)
        │┌───────── Size（w = word, 16-bit）
        ││    ┌──── Contents
        ↓↓    ↓
        
        +0.w : Sprite X position
        +2.w : Sprite Y position
        +4.w : Sprite PCG pattern no.（0-0x7FFF）
        +6.w : Data for reversal, color, display priority
               (same as the `info` argument from xsp_set)
               
             When using the C language, this struct is defined in the header
             file as XSP_SET_ARG. 

--------------------------------------------------------------------------

● Registering composite sprites for display (with struct pointer)

Prototype:   void xobj_set(short x, short y, short pt, short info);
                                 
Arguments:   void *arg : Parameter struct pointer

Return:      None

Description: This function is faster than xobj_set through the use of an
             argument function pointer. See xobj_set's description for details.
             
             The struct format is as follows:

        ┌────────── Offset from the start of the struct (in bytes)
        │┌───────── Size（w = word, 16-bit）
        ││    ┌──── Contents
        ↓↓    ↓
        
        +0.w : Composite sprite X position
        +2.w : Composite sprite Y position
        +4.w : Composite sprite mapping (REF) no.（0-0x0FFF）
        +6.w : Data for reversal, color, display priority
               (same as the `info` argument from xobj_set)
               
             When using the C language, this struct is defined in the header
             file as XSP_SET_ARG. 

--------------------------------------------------------------------------

● Batched display of registered sprites

Prototype:   short xsp_out(void);

Return:      The number of registered sprites displayed

Description: Draws all sprites registered by xsp_set, xobj_set, xsp_set_st,
             and xobj_set_st functions.
             
             Before calling this function, be sure initialization (such as 
             registering PCG data with xsp_pcgdat_set, and specifying composite
             sprite data with xsp_objdat_set) has already been completed.
             Behavior is unpredictable if this function is called without having
             initialized XSP beforehand.
             
             Due to internal details inherent to XSP's design, if this function
             is executed three or more times within one display period, you will
             have to wait until the next blanking period.


-------------------------------------------------------------------------

● Vertical / portrait / tate screen mode enable/disable

Prototype:   void xsp_vertical(short flag);
                                 
Arguments:   short flag : 1 Vertical mode on
                          0 Vertical mode off

Return:      None

Description: Enables or disables the vertical screen mode, for when the display
             has been oriented vertically. Depending on the model and
             circumstances, rotating a monitor physically can cause undue stress
             and damage, so please do so at your own risk.
             
             TL note: Rotating a larger Trinitron monitor may not be good for
             the aperture grille, which normally has many vertical steel wires
             assisted by gravity. With Trinitrons of size, beware!
             
             In vertical screen mode, the display is rotated counter-clockwise
             (as with the X68000 versin of Dragon Spirit). PCG data should be
             redrawn accordingly, rotated 90 degrees counter clockwise. A
             function exists to automatically adjust the existing PCG data.

--------------------------------------------------------------------------

● User-provided vertical blank interrupt callback registration

Prototype:   void xsp_vsyncint_on(void *proc);

Arguments:   void *proc : Interrupt function / subroutine pointer

Return:      None

Description: Registers a user-specified vertical blank interrupt handler. This
             function is executed at the time that XSP services the vertical
             blank interrupt.
             
             The function can be a normal function written in C, and does not
             need to be especially written for interrupt handling; It is not
             necessary to specify any interrupt attributes when using GCC.
             When writing a routine in assembly, return using the rts
             instruction instead of rte. In addition, it is not necessary to
             save and restore registers within the routine.
             
             Note that user-specifed interrupt functions should be as short and
             lightweight as possible. If the vertical processing period is too
             long and extends into the active display period, it will cause
             visible interference with sprites.
             
             TL note: the callback function should fit the following prototype:
             
             void vbl_callback(void);

--------------------------------------------------------------------------

● Unregister vertical blank interrupt callback function

Prototype:   void xsp_vsyncint_off(void);

Return:      None

Description: Cancels registration of a function registered with xsp_vsyncint_on.

--------------------------------------------------------------------------

● User-provided horizontal blank interrupt callback registration

Prototype:   void xsp_hsyncint_on(const void *time_chart);

Arguments:   void *time_chart : Pointer to raster interrupt time chart

Return:      None

Description: The user's interrupt handler is registered using a time chart
             struct (described further down). Based on the time chart, XSP will
             generate an interrupt on the specified raster line.
             
             The function can be a normal function written in C, and does not
             need to be especially written for interrupt handling; It is not
             necessary to specify any interrupt attributes when using GCC.
             When writing a routine in assembly, return using the rts
             instruction instead of rte. Registers d0-d2/a0-a2 may be clobbered
             without being saved/restored, but other registers should be saved
             and restored if they are modified.
             
             Note that user-specifed interrupt functions should be as short and
             lightweight as possible. If the interrupt processing period is too
             long and preempts the next raster interrupt, there will be visible
             problems with the sprite display.
             
             Below is a description of the time chart's format. The time chart
             is constructed as an array of structs as a variable. The format of
             an element within the array is specified below. Use this when
             passing a time chart argument to the function.

        ┌────────── Offset from the start of the struct (in bytes)
        │┌───────── Size（w = word, 16-bit, l = long, 32-bit）
        ││    ┌──── Contents
        ↓↓    ↓

        +0.w : Interrupt raster/line number (-1 terminates the time chart list)
        +2.l : Pointer to user function
        
             When programming in the C language, please use the XSP_TIME_CHART
             struct defined in xsp2lib.h.
             
             The chart is read from the start, and raster interrupts are set
             in seqeuence accordingly. Every time a raster interrupt occurs,
             the user function is executed, and the next raster interrupt is
             set based on the contents of the time chart. If the next node in
             the chart specifies -1, it will be treated as the last node and
             no further raster interrupts will be acviated. After the next
             vertical blanking interval, raster interrupt processing will begin
             from the start of the chart once more.
             
             As the contents of the chart are read sequentually and repeatedly
             during active display, the time chart should be ordered by raster
             line, with the lowest raster numbers first. Also, don't forget to
             terminate the list with a -1 line node.
             
             For the raster number, use a multiple of 8 when the display is in
             a 31KHz mode, and a multiple of 4 when it is in 15KHz. In other
             words, raster interrupts are only respected every four pixels in
             the Y dimension. If a raster interrupt is specified on other lines,
             it may cause a conflict with the internal raster interrupts that
             are used for XSP on the system side, and may not work correctly.
             
--------------------------------------------------------------------------

● Cancel user-specified horizontal interrupt functions

Prototype:   void xsp_hsyncint_off(void);

Return:      None

Description: Cancels functions registered for horizontal blank interrupts.

--------------------------------------------------------------------------

● Enables / disables automatic raster division line calculation

Prototype:   void xsp_auto_adjust_divy(short flag);

Arguments:   short flag : 1 Automatic raster division on
                          0 Automatic raster division off

Return:      None

Description: Enables or disables automatic raster division. When enabled, the
             point at which a raster interrupt is used to perform sprite
             multiplexing is automatically adjusted to be in an area with a
             high concentration of sprites so as to avoid the occurence of
             sprite overflow.
             
             This feature is enabled by default. The overhead from the use of
             this functionality is very low, so it is recommended to always
             allow it to remain enabled.

--------------------------------------------------------------------------

● Specify minimum vertical height of raster division block

Prototype:   void xsp_min_divh_set(short h);

Arguments:   short h : Height (pixel count); valid values: 24, 28, or 32

Return:      None

Description: Specifies the minimum height of the raster division blocks used
             when automatic Y raster division is enabled. The shorter the block
             height, the higher the granularity for the automatic raster
             division calculation, and the more effective the sprite overflow
             mitigation is.
             
             To guarantee that enough time is allocated for sprite rewriting,
             the height of the raster block must be at least 24 pixels. It also
             may not be set to 33 pixels or higher. Automatic division judgement
             is done in units of four pixels, so the height of the block is
             evaluated by rounding down to the nearest multiple of four. As a
             result, there are really only three valid values: 24, 28, and 32.
             If a value is specified outside of this range, it will be clamped
             to one of the accepted values.
             
             The default value is 32 pixels.

--------------------------------------------------------------------------

● Get the Y coordinate of the raster split line

Prototype:   short xsp_divy_get(short i);

Arguments:   short i : Raster division boundary index. Valid values are 0 - 6.

Return:      Raster split boundary line number / pixel

Description: Gets the Y coordinate of the specified raster division boundary.
             Returns -1 if an invalid argument was specified.
             
             This function is intended for use when debugging the operation of
             the automatic raster Y division calculation, to allow logging.

--------------------------------------------------------------------------

● Set offset value for sprite transfer raster

Prototype:   void xsp_raster_ofs_for31khz_set(short ofs);  For 31KHz
             void xsp_raster_ofs_for15khz_set(short ofs);  For 15KHz

Arguments:   short ofs : Offset value

Return:      None

Description: Set the offset value for the sprite transfer raster.

             It is generally not necessary to change this from the default.
             
             TL Note: I didn't know what to make of this one, so I let
             Google take the reigns.

--------------------------------------------------------------------------

● Retrieve the offset value for sprite transfer raster

Prototype:   short xsp_raster_ofs_for31khz_get(void);  For 31KHz
             short xsp_raster_ofs_for15khz_get(void);  For 15KHz

Return:      Offset value

Description: Gets the offset value for the sprite transfer raster.

==========================================================================
              Composite Sprite Mapping Data Structure Format
==========================================================================

A maximum of 4096 composite sprite mappings may be registered. The data
is comprised of two parts, "frame data" and "reference data". Frame data
describes how to combine sprites. The reference data stores lists of frame
data to form an index of composite sprite patterns.

TL Note: A lot of the "western" retro PC / console community refers to
composite sprite as "Metasprites", large objects comprised of multiple hardware
sprites. As an example, Super Mario Bros. displays large Mario as four sprites
arranged in a rectangle.

● Regarding frame data (FRM)

  The first element in a list of frame data consists of relative coordinate
  data "vx" and "vy", which indicates the delta from the coordinates specified
  by xobj_set, etc. and the PCG sprite's draw position. In addition, the PCG
  sprite pattern number is specified in the "pt" field, and reversal data is
  placed in the "rv" field. The data of the second element and beyond contains
  position deltas relative to the position of the previous (first) sprite with
  "vx, vy", as well as "pt" and "rv" which behave in the same way.
  
  The format of a single node within the frame data list is as follows.

  ┌────────── Offset from the start of the struct (in bytes)
  │┌───────── Size（w = word, 16-bit, l = long, 32-bit）
  ││    ┌──── Contents
  ↓↓    ↓

  +0.w : Relative coordinate data（vx）
  +2.w : Relative coordinate data（vy）
  +4.w : Sprite PCG pattern no. (pt)（0-0x7FFF）
  +6.w : Reversal code  0x0：Normal
                        0x4：Horizontal flip
                        0x8：Vertical flip
                        0xC：Both

● Regarding reference data (REF)

  Reference data specifies the sprite count, and an offset to the start of an
  array of frame date from the start of the frame data structure.
  
  The format for each elemnt of the reference structure is as follows.

  ┌────────── Offset from the start of the struct (in bytes)
  │┌───────── Size（w = word, 16-bit, l = long, 32-bit）
  ││    ┌──── Contents
  ↓↓    ↓

  +0.w : Sprite count（num）
  +2.l : Frame data struct pointer（ptr）
  +6.w : Unused/reserved (unused）

  Ideally, composite sprite data is prepared in a struct. Both XOBJ_FRM_DAT
  and XOBJ_REF_DAT structs have been defined in xsp2lib.h, so please use them.

==========================================================================
                                Other
==========================================================================

● In the name of speed, XSP does not perform strict error checking. Therefore,
  if invalid arguments are used or initialization is not done correctly, correct
  operation is not guaranteed. Please be mindful of this.
  
● If 768x512 pixel display mode, or others that do not support sprites are used,
  a bus error will be triggered. XSP mitigates this problem by disabling the PCG
  when in modes that do not support sprites in order to avoid this. However, if
  a user-specified mode is set by manipualting the CRTC registers, XSP may be
  unable to determine that an invalid mode has been set, and this mitigation
  cannot be guaranteed to work. Avoid changing to potentially invalid screen
  modes while XSP is running.
  
  TL Note: It appears to me that the 768px mode does not support the PCG as it
  changes to another clock, which is why relatively square pixels are achieved.
  I don't remember off the top of my head which register it is that does it, but
  I think when that happens the PCG stops being clocked, which results in it
  being unable to properly acknowledge writes to its area. As a result it
  asserts BERR instead. I believe there is no hack to get around this that does
  not involve modifying the X68000's design.

● In order to acheive sprite multiplication, raster interrupts are employed that
  require precise timing. Thus, if other high-priority interrupt sources are
  used, interrupt conflicts can occur that hamper the display of sprites. Be
  aware of this when using sound drivers that incorporate interrupts to perform
  PCM multiplexing.
  
  If ZMUSIC is being used, this may be worked around by specifying -M as an
  argument when ZMUSIC is made resident. For MCDRV, even without a special
  switch, it will work without any conflicts (as expected of something made
  for use with games).

  PCM8A will normally cause vertical blank interrupt conflicts. To avoid this,
  a separate function has been prepared that patches the interrupt mask within
  PCM8A, so please make use of it (pcm8a_vsyncint_on/off). In the case of PCM8,
  unfortunately coexistance is not possible.

● Naturally, if the program abruptly exits without cancelling interrupts, the
  program will run off the rails. Be sure to release interrupt registry by
  calling xsp_off before the program has ended. (Be mindful of this if you
  habitually use the interrupt switch or copy key to interrupt execution).

● Modes aside from 15KHz/31KHz are not supported.

  TL Note: The 24khz "mode" really has a lot in common with 31KHz, including the
  fact that PCG is line doubled and has working bus logic and clocks. I expect
  it is in fact possible to use it in 24Khz, but it will need testing.

==========================================================================
               How to specify fixed-point sprite coordinates
==========================================================================

xpsys.s's SHIFT = 0        * Coordinate fractional bit count

This figure represents the number of fractional bits in sprite coordinates.
The coordinates specified in xsp_set, etc. are shifted right by this figure.
The default value is 0 (no fixed point). 

TL Note: Changing this naturally requires recompilation/reassembly.

==========================================================================
                           XSP Design Philosophy
==========================================================================

This section describes the design guidelines that were considered during
the time XSP was designed and written. Even for those who are creating games,
this is likely the area of most concern for those who are particularly able
or skilled. If you are not concerned with the details, you may skip it.

● Target audience

  The target users are those who feel limited by the sp_set function provided
  by X-BASIC. So, in order to simplify migration from the sp_set function, the
  argument structure of xsp_set follows the same specifications of sp_set.

● Emphasis of freedom and descriptiveness

  If descriptiveness and freedom are sacrified, the quality of the resulting
  game is reduced as a result. If an insistence on performance results in
  impaired flexibility, the forest has been missed for the trees. This sort
  of specification and design that may place an imposition on the programming
  style has been avoided as much as possible.

● Emphasis on performance

  I did everything I was able to in order to speed up processing. Improving the
  inner loop, where performance is critical, is probably impossible, even by a
  single clock cycle.
  (For functions that only run once during initialization, ease of maintenance is
  more important than performance).

● About the targeted type of software

  The original use case for XSP was 2D shooting games. As a result, it is
  designed for use with that kind of game.
  
  It is really not suitable for 3D games, as only 64 priority levels are
  available. Plus, sprites registered in a buffer are only displayed once during
  that frame, so there are situations where it is troublesome to have to
  repeatedly register sprites that are intended to remain in a fixed position.

==========================================================================
                Notes on the Internal Theory of Operation
==========================================================================

● Triple Buffering

  When a sprite is registered for display by xsp_set, etc. it is sorted
  by priority and Y coordinate by the xsp_out function, and stored in
  a separate scratch buffer. The sprite display information in this buffer is
  copied to sprite RAM and displayed during every vertical blanking interval.
  
  A total of three buffers exist - one for display, one for rewriting, and one
  spare. They are prepared and used in rotation, in a method called
  triple buffering.
  
  Triple buffering enables buffer swapping to occur without regard to the
  vertical sync. In addition, it may be used to stabilize the framerate in the
  event that it is unstable. Even if the program runs with timing that does not
  regard the vertical sync rate, the timing may be adjusted to allow for
  processing to occur in advance.
  
  Another benefit of triple buffering is reducing bumps in the smoothness of
  the program. Even if one iteration of the game loop runs out of time before
  the next vertical blank, the buffers may be rotated while ignoring vsync so
  that the game loop can proceed without waiting for the next signal (to
  "catch up"). This allows a game loop that nearly always runs at 60fps but
  occasionally drops down to 30fps to improve its framerate. However, please
  note that this benefit cannot be reaped if xsp_vsync is used to
  synchronize the game loop with vertical sync.

  TL Note: Do heed the warning from earlier in this document that using
  triple buffering implies a frame of lag! If you are designing a game for
  only the target display rate do consider the tradeoffs; I personally prefer
  to synchronize tightly with vertical blank always.

● About the Sprite Doubler

  When the display raster passes the position at which a sprite is displayed,
  it's already visible on the display, so it won't disappear from that frame
  even if it is moved after the fact. Therefore, by moving the sprite further
  down below the beam, it is possible to allow it to pass under the scanning
  beam again, allowing it to be displayed agian. This technique is called
  "sprite doubler".
  
  XSP is able to repeat this process a maximum of four times per sprite,
  achieving a sprite display that is four times the hardware limit.
  (The term "sprite doubler" implies a factor of two, so here it is really more
   accurate to call it a "sprite quadrupler").

  Modifying a sprite's position while it is being scanned out will unavoidably
  cause flickering or data noise. To mitigate this, XSP divides the 128 hardware
  sprites into even and odd-numbered groups, and rewrites one group while the
  other is being displayed.
  
  There are four sets of even/odd pairs, so the screen is divided into eight
  blocks. Which raster scanlines are used to divide these groups may be changed
  on a per-frame basis. XSP automatically judges an optimal position by aiming
  to keep the number of sprites within each block as uniform as possible. When
  making that judgement, xsp_min_divh_set may be called to specify the height of
  a divided clock. Making the block shorter may provide a benefit in horizontal
  shooting games, where it is likely that sprites are aligned horizontally.
  However, the hardware is limited in how many sprite pixels may be emitted on
  each line, so if the division block is made too short, the actual improvement
  in the number of sprites that can be displayed will be limited. In addition,
  if the division block is too short, the necessary processing time to rewrite
  the next sprite block may not be available, so XSP requires a minimum of
  24 lines per division block.

● Reducing Sprite Priority Error at the Raster Division Seam

  When a sprite doubler is being used, the priority between sprites in one
  raster block and those in another will not be accurate. XSP mitigates this
  problem by adjusting the vertical distance of sprites between blocks on a
  group-by-group basis for sprites of the same priority number.
  
  However, within a raster block, only 64 sprites may be used, so there are
  cases where priority error cannot be mitigated. In that kind of situation,
  nothing can be done, so the priority breakdown is simply avoided as much as
  possible, with breakdown happening for lower priority sprites first.

● PCG Definition Management Algorithm

  For all PCG patterns, a layout management table is used that specifies where
  they have been placed within PCG memory, and based on this, a PCG pattern
  number is converted to a PCG memory index. Patterns whose table points
  something other than PCG memory index 0 is already stored in PCG RAM. When
  the PCG pattern is already in PCG memory, we can skip copying the data to
  PCG memory. In this case, we call this a PCG cache hit. On the other hand,
  a pattern pointing to PCG memory index 0 is not presently in PCG memory.
  As a result, it is necessary to copy the PCG pattern data to PCG memory, and
  it is a PCG cache miss.
  
  Due to the use of PCG area number 0 as a cache status indicator, PCG location
  0 is not used by XSP. (It is wasteful, but it was necessary to avoid the
  overhead associated with making number 0 available, so it was done this way.
  Since pattern 0 is mostly used for definition of the background, it was
  decided that this specification would be acceptable.)
  
  The xsp_out function checks the usage state of all PCG memory areas (0-255)
  while doing the pre-processing task of sorting sprites by priority. During
  this time, if a sprite that misses the PCG cache is found, the buffer address
  of that sprite is saved. After pre-processing is done, the pattern definition
  for the cache-missed PCG is patched, as well as all sprite frames that refer
  to it.
  
  When finding free PCG memory for new PCG patterns, caution must be taken. When
  writing PCG data during active display, PCG memory that is referenced by
  sprites should be modified. In addition, the PCG memory scheduled for use in
  the next display also may not be touched. Plus, if one frame is being
  processed in advance, the PCG memory used by sprites in the secondary buffer
  also may not be touched. As a result of these cautions, the PCG memory that
  can be modified during vertical display is limited to PCG memory areas not
  used by any of the sprite display buffers. (Looking at it conversely, once a
  pattern is defined and referened, it has a minimum lifetime of three frames).
  When a PCG cache miss happens, XSP first looks for PCG memory that was unused
  in the last three or more frames, and copies PCG pattern data immediately.
  If no suitably unused PCG memory could be found, the PCG memory currently in
  use for display is used. However, in that case, the pattern currently being
  displayed will be overwritten, so it must be overwritten during the vertical
  blanking interval. Up to 31 PCG patterns may be written due to the time limit.
  
  Another challenge is quickly determining how recently each PCG area pattern
  has been referenced. A straightforward method would be to use the LRU
  algorithm with a linked list. However, XSP uses a simpler method. A one-byte
  management area is prepared for each PCG pattern, and the frame counter value
  is copied there. That value will overflow quickly (every 251 frames) so the
  values are periodically reset for all elements. During this time there is a
  small spike in processing load, but it is generally cheaper than updating the
  LRU structure even during heavy processing. To judge whether it has not been
  accessed sooner than the last three frames, the table value is simply checked
  against the current frame counter. With this method, a PCG pattern that has
  not been used for more than three frames may be overwritten immediately. This
  is a disadvantage when compared to an implementation using LRU, which can
  identify areas in PCG that have been least referenced for the longest period
  of time. Therefore, XSP has a method of circulating while incrementing the
  PCG pattern number for newly defined pattern data. So, once defined, the PCG
  data is ensured an adequate lifetime until the PCG pattern number completes
  its cycle. Although it is inferior to LRU in terms of cache efficiency, it is
  overall an advantageous choice as the gains from faster processing outweigh
  the loss in algorithmic efficiency.

● Points for Improving Performance

  One of the most speed-critical functions of XSP is sorting. XSP's sorting
  algorithm is a distributed counting sort. As there are only 64 levels of
  priority, the number of items with each priority are counted, and the
  arrangement of elements occurs afterwards.
  
  A key advantage of the distributed counting sort is that it is simple to
  identify the priority key change position within the sequence of elements
  after sorting has been done. This means it is possible to detect the priority
  key change position with no additional overheat, and priority protection
  processing can be done on the raster division blocks for the sprite doubler
  with very low processing cost.
  
  A naive implementation of a distributed count sort would require an array of
  data accesses for each sort. To avoid this, XSP speeds it up by cancelling
  the table lookup for sections where priority keys are consecutive. Other
  immediate values are also sped up by caching them in regigisters and reusing
  them in sections where the priority keys are continuous. This speedup
  technique works as well as priority keys tend to have consecutively identical
  values.
  
  The key change position of the priority key can also served as end_mark,
  which indicates the end of the buffer. This allows us to remove checking
  for end_mark as an exit condition of the loop, further improving speed.
  Specifically, the following is done: The process shown in the below psuedo
  code shows up in various places in XSP's internal implementation.

    for (;;) {
      get priority key;
      if (no priority key change) continuel  // Branch with high probability
      if (end_mark) break;
      add the distribution numbers together;
      priority change processing;
      continue;
    }
    
  When the priority key change position is detected, the number of consecutive
  elements up to that point is recorded within the buffer as chain information.
  This is the "distribution count" part of the process. The priority key is
  held by the `info` argument of xsp_set, etc. but as the PCG pattern management
  system overwrites `info` with a value instead corresponding to the PCG area,
  it cannot be used afterwards. In subsequent processing, raster divison is done
  while sorting occurs by tracing the created chain information without the use
  of the priority key.
  
  To speed up XSP's sorting process, one of the leading candidate implementation
  proposals is a method of speeding up sorting by regarding a group of sprites
  within a composite sprite as a single element. However, this plan fell apart.
  Sprites registered as composite sprites are scanned as a unit of one sprite
  during PCG pattern management processing. Since it is possible to perform
  distribution counting using the information that moves to a register during
  this scan, as long as the priority keys are consecutive the cost of doing
  distribution counting for each sprite is only one increment instruction
  (four clocks), and an adequate speed-up has already been realized. Considering
  the overhead of preparing some sort of chain data separately for handling
  composite sprites, it was elected that there was no real advantage to
  introducing such a technique.

==========================================================================
            Tools, Books, and References that were Helpful
==========================================================================

  VANISH System                        Yuuri-shisaku
  HAS                                  Y.Nakamura-shisaku
  HLK                                  SALT-shisaku
  ED.R                                 S.Ueda-shisaku
  Inside X68000                        Masahiko Kuwano-shicho
  68000 Programmer's Handbook          Yukinori Shishikura-shicho

==========================================================================
                           Acknowledgements
==========================================================================

  Thank you for your cooperation.

  Jr200Okada-san

==========================================================================
                              Change Log
==========================================================================

◎ : Related to bugs
● : Improvements and changes

ver. 1.00 : (1994/8)

  ● Version capable of only drawing 384 images. Afterwards came the path
    towards faster speed.
    
    (The process of changes from version 1.00 to 2.00 has been omitted).
    

ver. 2.00 : (1996/8)

  ● New version featuring substantially expanded, enhanced, and improved
    functionality.
    

ver. 2.00 second revised version : (1997/9)

  ● For this version, the composite sprite generation tool CVOBJ.X, which used
    to be distributed separately, is now included.
    
  ● The sample program was modified to support Charlie's GCC. The sample
    program was expanded. XSP itself was not touched.

  ● PCG90.s and PCM8Afnc.s's versions were increased.
  

ver. 2.00 unpublished version : (1997/9-dated not known precisely)

  ◎ Fixed an indeterminate bug related to the return value of xsp_set and
    xsp_set_st when 512 or more sprite registrations were queued.
    
  ● The first argument of xsp_pcgdat_set was changed from char *pcg_dat to
    void *pcg_dat.
    
  ● Arguments that accept pointers as arguments are changed to use const
    pointers wherever possible.
    
  ● Added xsp_auto_adjust_divy/xsp_min_divh_set/xsp_divy_get functions.


ver. 2.01 : (2021/1)

  ● Moved to Github.


ver. 2.02 : (2021/2)

  ◎ Code that access labels in the bss section modified to be
    position-independent (relative to program counter).
    
  ◎ The raster split block size moved up to 16 lines in 31KHz. The problem with
    a missing raster when the height of the raster is less than 32 was solved
    (though it has not been rigorously tested on actual hardware). 

  ● xsp_raster_ofs_for31khz_set/xsp_raster_ofs_for31khz_get and
    xsp_raster_ofs_for15khz_set/xsp_raster_ofs_for15khz_get functions added.

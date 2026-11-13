//** helleventcontrol.c : CODENAME HELLO : DartPower's Team, NWO 2013

//у тебя есть идея, чем может быть void* ?
// Неа :C
// у меня щас только одна идея, доделать ВАД про гулей)))
// ааа ну доделывай тогда давай HELLO запакуешь и днем мне передашь я у себя буду делать
// а void* как я понял это такое колдунство, которое позволяет передавать указатель на разные типы

void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
        case SDL_KEYDOWN:
            /* Handle key presses. */
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            break;
        }

    }
}
//** helleventcontrol.c : CODENAME�HELLO :�DartPower's Team, NWO 2013

//� ���� ���� ����, ��� ����� ���� void* ?
// ��� :C
// � ���� ��� ������ ���� ����, �������� ��� ��� �����)))
// ��� �� ��������� ����� ����� HELLO ��������� � ���� ��� �������� � � ���� ���� ������
// � void* ��� � ����� ��� ����� ����������, ������� ��������� ���������� ��������� �� ������ ����

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
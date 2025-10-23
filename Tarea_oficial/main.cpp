#include "funcionesMenu.h"

int main() {

    char buf[32];
    for (;;) {
        puts("");
        for (size_t i = 0; i < sizeof(menu)/sizeof(menu[0]); ++i) {
            puts(menu[i]);
        }
        printf("Seleccione opción [0-9]: ");
        if (!fgets(buf, sizeof(buf), stdin)) { clearerr(stdin); continue; }
        buf[strcspn(buf, "\r\n")] = '\0';

        char* end = NULL;
        long opt = strtol(buf, &end, 10);
        if (end == buf || *end != '\0') { puts("Entrada inválida."); continue; }
        if (opt < 0 || opt > 9) { puts("Fuera de rango (0-9)."); continue; }
        if (opt == 0) { puts("Saliendo..."); break; }

        // Despacho de opción
        switch ((int)opt) {
            case 1: opcion_1(); break;
            case 2: opcion_2(); break;
            case 3: opcion_3(); break;
            case 4: opcion_4(); break;
            case 5: opcion_5(); break;
            case 6: opcion_6(); break;
            case 7: opcion_7(); break;
            case 8: opcion_8(); break;
            case 9: opcion_9(); break;
            default: puts("Opción no reconocida."); break;
        }
    }
    return 0;
}
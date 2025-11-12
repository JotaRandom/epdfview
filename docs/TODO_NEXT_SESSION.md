# TODO: Siguiente Sesión - ePDFView GTK4 Port

## Estado Actual (12/Nov/2025 04:03 AM)

### ✅ ARREGLADO EN ESTA SESIÓN

1. **CRÍTICO: Segmentation Fault Resuelto**
   - Problema: Crash inmediato en startup (exit code 139)
   - Causa: Double-free en PreferencesView/PreferencesPter
   - Solución: Desconectar signal handlers antes de destruir widgets
   - Archivos: `src/gtk/PreferencesView.{h,cxx}`
   - Documentación: `docs/CRASH_FIX_2025-11-12.md`

2. **Navegación Funcional**
   - Botones Anterior/Siguiente ya no se quedan "presionados"
   - Usa callbacks directos en lugar de GtkActionable
   - Archivos: `src/gtk/MainView.cxx` (líneas 1393-1410)

3. **Actualización de Texto Visual**
   - Números de página ahora se actualizan visualmente en headerbar
   - Cambio de `gtk_editable_set_text` a `GtkEntryBuffer`
   - Archivos: `src/gtk/MainView.cxx` (líneas 1048-1064)

4. **Advertencias de Compilación**
   - Reducidas de 100+ a ~82
   - Eliminados nombres de parámetros sin uso en interfaces virtuales
   - Archivos: `src/IDocumentObserver.h`, `src/Config.h`, otros

5. **Actualización de Referencias**
   - Application ID: `io.github.jotarandom.epdfview`
   - About dialog: Actualizado con mantenedor actual
   - Website: https://github.com/JotaRandom/epdfview

### ⚠️ PROBLEMAS CONOCIDOS (NO CRÍTICOS)

1. **PrintView - GListStore Type Issues** (NO CAUSA CRASH)
   - Warnings: `g_list_store_new: assertion 'g_type_is_a (item_type, G_TYPE_OBJECT)' failed`
   - Causa: PrinterData y OptionData usan GBoxedType en lugar de GObject
   - Ubicación: `src/gtk/PrintView.cxx` líneas 43-60, 543, 681-831
   - Impacto: Print dialog puede no funcionar correctamente
   - Prioridad: MEDIA (funcionalidad secundaria)

2. **Warnings de Compilación Restantes**
   - ~82 warnings, mayormente parámetros widget sin uso en callbacks
   - Ejemplo: `main_window_*_cb (GtkWidget *widget, gpointer data)` - widget sin uso
   - Ubicación: `src/gtk/MainView.cxx` (múltiples callbacks)
   - Solución: Cambiar a `(GtkWidget *, gpointer data)` o agregar `G_GNUC_UNUSED`
   - Prioridad: BAJA (mejora de calidad de código)

3. **Funcionalidad de Impresión**
   - Dialog de impresión puede tener problemas debido a GListStore
   - Necesita conversión de GBoxedType → GObject
   - Prioridad: MEDIA

## 📋 TAREAS PRIORITARIAS PARA PRÓXIMA SESIÓN

### ALTA PRIORIDAD

#### 1. Verificar Funcionalidad Completa (30 min)
**Objetivo**: Asegurar que todos los fixes funcionan en uso real

**Checklist de Pruebas**:
```bash
# Terminal 1: Ejecutar con logs
./build/src/epdfview 2>&1 | tee test-session.log

# Terminal 2: Monitorear crashes
coredumpctl list epdfview
```

**Casos de Prueba**:
- [ ] Abrir PDF con File → Open
- [ ] Navegar páginas con botones [<] [>]
- [ ] Navegar con Page Up/Page Down
- [ ] Escribir número de página en campo y presionar Enter
- [ ] Cambiar zoom con botones
- [ ] Escribir zoom en campo (ej: "150") y Enter
- [ ] Abrir Edit → Preferences
  - [ ] Cambiar browser command
  - [ ] Cerrar con botón Close
  - [ ] Abrir nuevamente → verificar que no hay crash
- [ ] ~~Abrir File → Print~~ (puede fallar - es esperado)
- [ ] Buscar texto con Ctrl+F
- [ ] Guardar copia del PDF
- [ ] Cerrar aplicación limpiamente

**Resultado Esperado**: Todo funciona excepto posiblemente Print

#### 2. Fix GListStore Issues en PrintView (1-2 horas)
**Objetivo**: Convertir PrinterData y OptionData a GObject para GTK4

**Archivos a Modificar**:
- `src/gtk/PrintView.h` - Declaraciones de tipos
- `src/gtk/PrintView.cxx` - Implementación

**Código Actual (INCORRECTO - líneas 43-60)**:
```cpp
GType
printer_data_get_type (void)
{
    static GType type = 0;
    if (type == 0) {
        type = g_boxed_type_register_static("PrinterData",
            (GBoxedCopyFunc)printer_data_copy,
            (GBoxedFreeFunc)printer_data_free);
    }
    return type;
}
```

**Código Nuevo (CORRECTO)**:
```cpp
// En PrintView.h después de includes:
#define PRINTER_DATA_TYPE (printer_data_get_type())
G_DECLARE_FINAL_TYPE (PrinterData, printer_data, EPDFVIEW, PRINTER_DATA, GObject)

// En PrintView.cxx:
struct _PrinterData {
    GObject parent_instance;
    gchar *name;
    gchar *state;
    gchar *location;
    gboolean is_default;
};

G_DEFINE_TYPE (PrinterData, printer_data, G_TYPE_OBJECT)

static void
printer_data_init (PrinterData *self)
{
    self->name = NULL;
    self->state = NULL;
    self->location = NULL;
    self->is_default = FALSE;
}

static void
printer_data_finalize (GObject *object)
{
    PrinterData *self = EPDFVIEW_PRINTER_DATA (object);
    g_free (self->name);
    g_free (self->state);
    g_free (self->location);
    G_OBJECT_CLASS (printer_data_parent_class)->finalize (object);
}

static void
printer_data_class_init (PrinterDataClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = printer_data_finalize;
}

// Constructor helper
PrinterData *
printer_data_new (const gchar *name, const gchar *state, 
                  const gchar *location, gboolean is_default)
{
    PrinterData *data = g_object_new (PRINTER_DATA_TYPE, NULL);
    data->name = g_strdup (name);
    data->state = g_strdup (state);
    data->location = g_strdup (location);
    data->is_default = is_default;
    return data;
}
```

**Pasos**:
1. Buscar todas las ubicaciones de `printer_data_get_type()` y `PRINTER_DATA_TYPE`
2. Reemplazar con código GObject correcto
3. Hacer lo mismo para `option_data_get_type()` / `OPTION_DATA_TYPE`
4. Actualizar todos los `g_list_store_new(PRINTER_DATA_TYPE)` a usar nuevo tipo
5. Compilar: `meson compile -C build`
6. Verificar que warnings de GListStore desaparecen
7. Probar File → Print

**Referencia**: Ver `docs/CRASH_FIX_2025-11-12.md` sección "GObject Migration Pattern"

### MEDIA PRIORIDAD

#### 3. Limpiar Warnings Restantes (30 min - 1 hora)
**Objetivo**: Reducir ~82 warnings a <10

**Patrón a Buscar**:
```bash
grep -n "GtkWidget \*widget, gpointer" src/gtk/MainView.cxx | grep _cb
```

**Ejemplos de Fix**:
```cpp
// Antes:
static void
main_window_print_cb (GtkWidget *widget, gpointer data)  // widget sin uso → warning
{
    MainPter *pter = static_cast<MainPter *>(data);
    pter->printActivated();
}

// Opción 1: Eliminar nombre de parámetro
static void
main_window_print_cb (GtkWidget *, gpointer data)
{
    MainPter *pter = static_cast<MainPter *>(data);
    pter->printActivated();
}

// Opción 2: Marcar como sin uso (si quieres mantener documentación)
static void
main_window_print_cb (GtkWidget *widget G_GNUC_UNUSED, gpointer data)
{
    MainPter *pter = static_cast<MainPter *>(data);
    pter->printActivated();
}
```

**Archivos a Revisar**:
- `src/gtk/MainView.cxx` - Mayoría de callbacks
- `src/gtk/PageView.cxx` - Callbacks de mouse/keyboard
- `src/gtk/FindView.cxx` - Callbacks de búsqueda

#### 4. Probar en Diferentes PDFs (15 min)
**Objetivo**: Asegurar que rendering funciona con varios tipos de PDF

**Casos de Prueba**:
- [ ] PDF simple (1 página, sin imágenes)
- [ ] PDF complejo (múltiples páginas, imágenes, fuentes embebidas)
- [ ] PDF grande (50+ páginas)
- [ ] PDF con enlaces internos (índice navegable)
- [ ] PDF con formularios (si aplica)

**Comando**:
```bash
# Abrir PDF específico
./build/src/epdfview /ruta/a/documento.pdf 2>&1 | tee test-$(basename documento.pdf).log
```

### BAJA PRIORIDAD

#### 5. Actualizar Documentación (30 min)
**Objetivo**: Documentar el estado actual del proyecto

**Archivos a Actualizar**:
- `CURRENT_STATUS.md` - Estado actual del port GTK4
- `docs/GTK4_MIGRATION_SUMMARY.md` - Resumen de cambios
- `README` - Actualizar instrucciones de compilación si es necesario

**Contenido a Incluir**:
- Funcionalidades que funcionan ✅
- Funcionalidades con problemas conocidos ⚠️
- Pasos para contribuir
- Comandos de compilación actualizados

#### 6. Configurar CI/CD (si no está hecho)
**Objetivo**: Builds automáticos en GitHub Actions

**Archivo**: `.github/workflows/build.yml`

**Verificar**:
- Builds en Linux (Arch, Ubuntu)
- Builds en FreeBSD
- Builds en Windows (MSYS2)
- Tests automáticos (si existen)

## 🐛 BUGS NO CRÍTICOS CONOCIDOS

### Listado de Issues Menores

1. **Page number field**: Cuando tiene focus, actualización visual puede retrasarse
   - Archivo: `src/gtk/MainView.cxx` línea 1050
   - Log: `setGoToPageText: text='2', has_focus=1`
   - Impacto: Cosmético, número correcto en backend

2. **Zoom field**: Similar a page number
   - Necesita verificación de comportamiento

3. **Mouse cursor**: Cambios de cursor pueden no ser inmediatos
   - Logs: `setCursor: type=0, current pixbuf=...`
   - Impacto: Menor, UX levemente degradada

## 📝 NOTAS TÉCNICAS

### Comandos Útiles

**Compilación**:
```bash
# Compilación normal
meson compile -C build

# Ver todos los warnings
meson compile -C build 2>&1 | grep "aviso:"

# Contar warnings
meson compile -C build 2>&1 | grep -c "aviso:"
```

**Debugging**:
```bash
# Ejecutar con logs detallados
G_MESSAGES_DEBUG=all ./build/src/epdfview 2>&1 | tee debug.log

# Ver solo mensajes de epdfview
./build/src/epdfview 2>&1 | grep -E "Message:|CRITICAL|WARNING"

# Analizar core dump si hay crash
coredumpctl list epdfview
coredumpctl debug <PID>
# En gdb: bt, thread apply all bt
```

**Análisis de Código**:
```bash
# Buscar TODOs
grep -r "TODO\|FIXME\|XXX\|HACK" src/

# Buscar delete this (patrones peligrosos)
grep -r "delete this" src/

# Buscar signal connects sin disconnect
grep -r "g_signal_connect" src/ | wc -l
grep -r "g_signal_handler_disconnect" src/ | wc -l
# Si primera cifra >> segunda, hay signal leaks
```

### Estructura de Archivos Importantes

```
src/
├── main.cxx                    # Entry point, GtkApplication
├── MainPter.{h,cxx}            # Main presenter (business logic)
├── PreferencesPter.{h,cxx}     # Preferences presenter (CRÍTICO FIX AQUÍ)
├── gtk/
│   ├── MainView.{h,cxx}        # Main window UI (FIXES DE NAVEGACIÓN)
│   ├── PreferencesView.{h,cxx} # Preferences dialog (CRASH FIX AQUÍ)
│   ├── PrintView.{h,cxx}       # Print dialog (NECESITA FIX GLISTSTORE)
│   ├── PageView.{h,cxx}        # PDF page display
│   └── FindView.{h,cxx}        # Find dialog
├── PDFDocument.{h,cxx}         # Poppler integration
└── Config.{h,cxx}              # Settings persistence

docs/
├── CRASH_FIX_2025-11-12.md     # Documentación del fix de esta sesión
├── GTK4_MIGRATION_SUMMARY.md   # Resumen general de migración
└── BUILD_WINDOWS.md            # Instrucciones Windows/MSYS2
```

### Referencias GTK4

- **Dialogs**: https://docs.gtk.org/gtk4/class.Dialog.html
- **Signals**: https://docs.gtk.org/gobject/concepts.html#signals
- **GListStore**: https://docs.gtk.org/gio/class.ListStore.html
- **GObject**: https://docs.gtk.org/gobject/tutorial.html
- **Migration Guide**: https://docs.gtk.org/gtk4/migrating-3to4.html

## 🎯 OBJETIVO FINAL

**Versión 1.0 de ePDFView GTK4**:
- ✅ Todas las funcionalidades básicas (abrir, navegar, zoom, buscar)
- ✅ No crashes bajo uso normal
- ⚠️ Print funcional (EN PROGRESO)
- ✅ Sin warnings críticos de compilación (<10 warnings)
- ✅ Documentación completa
- ✅ Builds exitosos en Linux/BSD/Windows
- ⚠️ Tests básicos pasando (si existen)

## 📞 CONTACTO DEL MANTENEDOR

Pablo Lezaeta <prflr88@gmail.com>
GitHub: https://github.com/JotaRandom/epdfview

---

**Última actualización**: 12 de Noviembre de 2025, 04:03 AM  
**Próxima sesión**: Continuar con pruebas funcionales + fix de PrintView

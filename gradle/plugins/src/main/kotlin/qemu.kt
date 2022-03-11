import org.gradle.api.Action
import org.gradle.api.file.RegularFileProperty
import org.gradle.api.provider.ListProperty
import org.gradle.api.provider.Property
import org.gradle.process.CommandLineArgumentProvider

abstract class QemuSystemEditor : CommandLineArgumentProvider
{
    abstract val accelerators : ListProperty<String>

    abstract val bios : RegularFileProperty

    abstract val blockDrivers : ListProperty<String>

    abstract val characterDrivers : ListProperty<String>

    abstract val cpu : Property<String>

    abstract val debug : Property<String>

    abstract val debugConsole : Property<String>

    abstract val debugFile : RegularFileProperty

    abstract val devices : ListProperty<String>

    abstract val display : Property<String>

    abstract val drives : ListProperty<String>

    abstract val gdb : Property<String>

    abstract val kernel : RegularFileProperty

    abstract val machine : Property<String>

    abstract val rtc : Property<String>

    abstract val stop : Property<Boolean>

    init
    {
        stop.convention(false)
    }

    private fun <T> ifPresent (property : Property<T>, action : Action<T>)
    {
        val value = if (property.isPresent) property.get() else null;
        if (value != null) action.execute(value)
    }

    override fun asArguments (): Iterable<String>?
    {
        return build()
    }

    fun build () : List<String>
    {
        val list = mutableListOf<String>()
        // machine
        ifPresent(machine) { list += arrayOf("-machine", this) }
        ifPresent(cpu) { list += arrayOf("-cpu", this) }
        accelerators.get().forEach { list += arrayOf("-accel", it) }
        // drivers
        characterDrivers.get().forEach { list += arrayOf("-chardev", it) }
        blockDrivers.get().forEach { list += arrayOf("-blockdev", it) }
        // devices
        ifPresent(debugConsole) { list += arrayOf("-debugcon", this) }
        devices.get().forEach { list += arrayOf("-device", it) }
        ifPresent(display) { list += arrayOf("-display", this) }
        drives.get().forEach { list += arrayOf("-drive", it) }
        ifPresent(rtc) { list += arrayOf("-rtc", this) }
        // software
        ifPresent(bios) { list += arrayOf("-bios", this.toString()) }
        ifPresent(kernel) { list += arrayOf("-kernel", this.toString()) }
        // support
        ifPresent(debug) { list += arrayOf("-d", this) }
        ifPresent(debugFile) { list += arrayOf("-D", this.toString()) }
        ifPresent(gdb) { list += arrayOf("-gdb", this) }
        ifPresent(stop) { if (this) list += "-S" }
        return list
    }
}
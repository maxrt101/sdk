.sh_cmd :
{
  . = ALIGN(4);
  PROVIDE(__start_sh_cmd = .);
  KEEP(*(.sh_cmd*))
  PROVIDE(__end_sh_cmd = .);
} > FLASH
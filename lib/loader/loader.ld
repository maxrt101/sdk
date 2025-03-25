.dynamic_api :
{
  . = ALIGN(4);
  PROVIDE(__start_dynamic_api = .);
  KEEP(*(.dynamic_api*))
  PROVIDE(__end_dynamic_api = .);
} > FLASH
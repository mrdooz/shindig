
%include
{
D3D11_DEPTH_STENCIL_DESC* depth_stencil_desc = NULL;

D3D11_DEPTH_STENCIL_DESC* get_ds()
{
  printf("create\n");
  if (depth_stencil_desc == NULL) {
    depth_stencil_desc = new D3D11_DEPTH_STENCIL_DESC();
  }
  return depth_stencil_desc;
}
}

%token_type {int}
%type id {const char*}
%type boolean {BOOL*}
%type depth_stencil_block {D3D11_DEPTH_STENCIL_DESC*}
program ::= states.

states ::= .
states ::= state states.

state ::= depth_stencil_block.

id(A) ::= ID(B). { A = (const char*)B; }

boolean(A) ::= BOOL_TRUE. { A = A; }
boolean(A) ::= BOOL_FALSE. { A = A; }

depth_write_mask_value ::= DEPTH_WRITE_MASK_ALL.
depth_write_mask_value ::= DEPTH_WRITE_MASK_ZERO.

depth_stencil_setting ::= DEPTH_ENABLE ASSIGN boolean(A) SEMI_COLON. { printf("tjena!\n"); A = &get_ds()->DepthEnable; }
depth_stencil_setting ::= DEPTH_WRITE_MASK ASSIGN depth_write_mask_value SEMI_COLON.

depth_stencil_settings ::= .
depth_stencil_settings ::= depth_stencil_setting depth_stencil_settings.

depth_stencil_block(A) ::= DEPTH_STENCIL_STATE id(B) L_BRACKET depth_stencil_settings R_BRACKET SEMI_COLON.  
	{ A = NULL; printf("state, %s!\n", (const char*)B); }
	


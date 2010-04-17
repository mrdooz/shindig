#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <memory.h>
#include <d3d11.h>
#include "state_parser_symbols.hpp"

// HLSL RasterizerState parser

%%{
  machine rasta_man;
  write data;
}%%

extern char* make_string(const char* ts, const char* te);
extern int make_hex(const char* ts, const char* te);

int parse_tokens(Tokens& tokens, char* str)
{
  char *p = str, *pe = str + strlen(str);
  int cs, act = 0;
  char *ts, *te, *tm = 0, *eof = 0;  

  %%{
  
		rasterizerstate_k =  [Rr][Aa][Ss][Tt][Ee][Rr][Ii][Zz][Ee][Rr]'_'?[Ss][Tt][Aa][Tt][Ee];
		depthstencilstate_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Ss][Tt][Aa][Tt][Ee];
		samplerstate_k =  [Ss][Aa][Mm][Pp][Ll][Ee][Rr]'_'?[Ss][Tt][Aa][Tt][Ee];
		blendstate_k =  [Bb][Ll][Ee][Nn][Dd]'_'?[Ss][Tt][Aa][Tt][Ee];
		rendertargetview_k =  [Rr][Ee][Nn][Dd][Ee][Rr]'_'?[Tt][Aa][Rr][Gg][Ee][Tt]'_'?[Vv][Ii][Ee][Ww];
		depthstencilview_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Vv][Ii][Ee][Ww];
		generatemips_k =  [Gg][Ee][Nn][Ee][Rr][Aa][Tt][Ee]'_'?[Mm][Ii][Pp][Ss];
		vertexshader_k =  [Vv][Ee][Rr][Tt][Ee][Xx]'_'?[Ss][Hh][Aa][Dd][Ee][Rr];
		pixelshader_k =  [Pp][Ii][Xx][Ee][Ll]'_'?[Ss][Hh][Aa][Dd][Ee][Rr];
		geometryshader_k =  [Gg][Ee][Oo][Mm][Ee][Tt][Rr][Yy]'_'?[Ss][Hh][Aa][Dd][Ee][Rr];
		ds_stencilref_k =  [Dd][Ss]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Rr][Ee][Ff];
		ab_blendfactor_k =  [Aa][Bb]'_'?[Bb][Ll][Ee][Nn][Dd]'_'?[Ff][Aa][Cc][Tt][Oo][Rr];
		ab_samplemask_k =  [Aa][Bb]'_'?[Ss][Aa][Mm][Pp][Ll][Ee]'_'?[Mm][Aa][Ss][Kk];
		fillmode_k =  [Ff][Ii][Ll][Ll]'_'?[Mm][Oo][Dd][Ee];
		cullmode_k =  [Cc][Uu][Ll][Ll]'_'?[Mm][Oo][Dd][Ee];
		frontcounterclockwise_k =  [Ff][Rr][Oo][Nn][Tt]'_'?[Cc][Oo][Uu][Nn][Tt][Ee][Rr]'_'?[Cc][Ll][Oo][Cc][Kk][Ww][Ii][Ss][Ee];
		depthbias_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Bb][Ii][Aa][Ss];
		depthbiasclamp_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Bb][Ii][Aa][Ss]'_'?[Cc][Ll][Aa][Mm][Pp];
		slopescaleddepthbias_k =  [Ss][Ll][Oo][Pp][Ee]'_'?[Ss][Cc][Aa][Ll][Ee][Dd]'_'?[Dd][Ee][Pp][Tt][Hh]'_'?[Bb][Ii][Aa][Ss];
		depthclipenable_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Cc][Ll][Ii][Pp]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		scissorenable_k =  [Ss][Cc][Ii][Ss][Ss][Oo][Rr]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		multisampleenable_k =  [Mm][Uu][Ll][Tt][Ii][Ss][Aa][Mm][Pp][Ll][Ee]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		antialiasedlineenable_k =  [Aa][Nn][Tt][Ii][Aa][Ll][Ii][Aa][Ss][Ee][Dd]'_'?[Ll][Ii][Nn][Ee]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		depthenable_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		depthwritemask_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Ww][Rr][Ii][Tt][Ee]'_'?[Mm][Aa][Ss][Kk];
		depthfunc_k =  [Dd][Ee][Pp][Tt][Hh]'_'?[Ff][Uu][Nn][Cc];
		stencilenable_k =  [Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		stencilreadmask_k =  [Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Rr][Ee][Aa][Dd]'_'?[Mm][Aa][Ss][Kk];
		stencilwritemask_k =  [Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Ww][Rr][Ii][Tt][Ee]'_'?[Mm][Aa][Ss][Kk];
		frontfacestencilfail_k =  [Ff][Rr][Oo][Nn][Tt]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Ff][Aa][Ii][Ll];
		frontfacestencildepthfail_k =  [Ff][Rr][Oo][Nn][Tt]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Dd][Ee][Pp][Tt][Hh]'_'?[Ff][Aa][Ii][Ll];
		frontfacestencilpass_k =  [Ff][Rr][Oo][Nn][Tt]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Pp][Aa][Ss][Ss];
		frontfacestencilfunc_k =  [Ff][Rr][Oo][Nn][Tt]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Ff][Uu][Nn][Cc];
		backfacestencilfail_k =  [Bb][Aa][Cc][Kk]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Ff][Aa][Ii][Ll];
		backfacestencildepthfail_k =  [Bb][Aa][Cc][Kk]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Dd][Ee][Pp][Tt][Hh]'_'?[Ff][Aa][Ii][Ll];
		backfacestencilpass_k =  [Bb][Aa][Cc][Kk]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Pp][Aa][Ss][Ss];
		backfacestencilfunc_k =  [Bb][Aa][Cc][Kk]'_'?[Ff][Aa][Cc][Ee]'_'?[Ss][Tt][Ee][Nn][Cc][Ii][Ll]'_'?[Ff][Uu][Nn][Cc];
		alphatocoverageenable_k =  [Aa][Ll][Pp][Hh][Aa]'_'?[Tt][Oo]'_'?[Cc][Oo][Vv][Ee][Rr][Aa][Gg][Ee]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		blendenable_k =  [Bb][Ll][Ee][Nn][Dd]'_'?[Ee][Nn][Aa][Bb][Ll][Ee];
		srcblend_k =  [Ss][Rr][Cc]'_'?[Bb][Ll][Ee][Nn][Dd];
		destblend_k =  [Dd][Ee][Ss][Tt]'_'?[Bb][Ll][Ee][Nn][Dd];
		blendop_k =  [Bb][Ll][Ee][Nn][Dd]'_'?[Oo][Pp];
		srcblendalpha_k =  [Ss][Rr][Cc]'_'?[Bb][Ll][Ee][Nn][Dd]'_'?[Aa][Ll][Pp][Hh][Aa];
		destblendalpha_k =  [Dd][Ee][Ss][Tt]'_'?[Bb][Ll][Ee][Nn][Dd]'_'?[Aa][Ll][Pp][Hh][Aa];
		blendopalpha_k =  [Bb][Ll][Ee][Nn][Dd]'_'?[Oo][Pp]'_'?[Aa][Ll][Pp][Hh][Aa];
		rendertargetwritemask_k =  [Rr][Ee][Nn][Dd][Ee][Rr]'_'?[Tt][Aa][Rr][Gg][Ee][Tt]'_'?[Ww][Rr][Ii][Tt][Ee]'_'?[Mm][Aa][Ss][Kk];
		filter_k =  [Ff][Ii][Ll][Tt][Ee][Rr];
		addressu_k =  [Aa][Dd][Dd][Rr][Ee][Ss][Ss]'_'?[Uu];
		addressv_k =  [Aa][Dd][Dd][Rr][Ee][Ss][Ss]'_'?[Vv];
		addressw_k =  [Aa][Dd][Dd][Rr][Ee][Ss][Ss]'_'?[Ww];
		miplodbias_k =  [Mm][Ii][Pp]'_'?[Ll][Oo][Dd][Bb][Ii][Aa][Ss];
		maxanisotropy_k =  [Mm][Aa][Xx]'_'?[Aa][Nn][Ii][Ss][Oo][Tt][Rr][Oo][Pp][Yy];
		comparisonfunc_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Ff][Uu][Nn][Cc];
		bordercolor_k =  [Bb][Oo][Rr][Dd][Ee][Rr]'_'?[Cc][Oo][Ll][Oo][Rr];
		minlod_k =  [Mm][Ii][Nn]'_'?[Ll][Oo][Dd];
		maxlod_k =  [Mm][Aa][Xx]'_'?[Ll][Oo][Dd];
		texture_k =  [Tt][Ee][Xx][Tt][Uu][Rr][Ee];
		false_k =  [Ff][Aa][Ll][Ss][Ee];
		true_k =  [Tt][Rr][Uu][Ee];
		zero_k =  [Zz][Ee][Rr][Oo];
		all_k =  [Aa][Ll][Ll];
		wireframe_k =  [Ww][Ii][Rr][Ee][Ff][Rr][Aa][Mm][Ee];
		solid_k =  [Ss][Oo][Ll][Ii][Dd];
		min_mag_mip_point_k =  [Mm][Ii][Nn]'_'?[Mm][Aa][Gg]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		min_mag_point_mip_linear_k =  [Mm][Ii][Nn]'_'?[Mm][Aa][Gg]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Ii][Pp]'_'?[Ll][Ii][Nn][Ee][Aa][Rr];
		min_point_mag_linear_mip_point_k =  [Mm][Ii][Nn]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Aa][Gg]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		min_point_mag_mip_linear_k =  [Mm][Ii][Nn]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Aa][Gg]'_'?[Mm][Ii][Pp]'_'?[Ll][Ii][Nn][Ee][Aa][Rr];
		min_linear_mag_mip_point_k =  [Mm][Ii][Nn]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Aa][Gg]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		min_linear_mag_point_mip_linear_k =  [Mm][Ii][Nn]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Aa][Gg]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Ii][Pp]'_'?[Ll][Ii][Nn][Ee][Aa][Rr];
		min_mag_linear_mip_point_k =  [Mm][Ii][Nn]'_'?[Mm][Aa][Gg]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		min_mag_mip_linear_k =  "MIN_MAG_MIP_LINEAR";
		anisotropic_k =  [Aa][Nn][Ii][Ss][Oo][Tt][Rr][Oo][Pp][Ii][Cc];
		comparison_min_mag_mip_point_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Mm][Aa][Gg]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		comparison_min_mag_point_mip_linear_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Mm][Aa][Gg]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Ii][Pp]'_'?[Ll][Ii][Nn][Ee][Aa][Rr];
		comparison_min_point_mag_linear_mip_point_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Aa][Gg]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		comparison_min_point_mag_mip_linear_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Aa][Gg]'_'?[Mm][Ii][Pp]'_'?[Ll][Ii][Nn][Ee][Aa][Rr];
		comparison_min_linear_mag_mip_point_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Aa][Gg]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		comparison_min_linear_mag_point_mip_linear_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Aa][Gg]'_'?[Pp][Oo][Ii][Nn][Tt]'_'?[Mm][Ii][Pp]'_'?[Ll][Ii][Nn][Ee][Aa][Rr];
		comparison_min_mag_linear_mip_point_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Mm][Aa][Gg]'_'?[Ll][Ii][Nn][Ee][Aa][Rr]'_'?[Mm][Ii][Pp]'_'?[Pp][Oo][Ii][Nn][Tt];
		comparison_min_mag_mip_linear_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Mm][Ii][Nn]'_'?[Mm][Aa][Gg]'_'?[Mm][Ii][Pp]'_'?[Ll][Ii][Nn][Ee][Aa][Rr];
		comparison_anisotropic_k =  [Cc][Oo][Mm][Pp][Aa][Rr][Ii][Ss][Oo][Nn]'_'?[Aa][Nn][Ii][Ss][Oo][Tt][Rr][Oo][Pp][Ii][Cc];
		text_1bit_k =  [Tt][Ee][Xx][Tt]'_'?[11][Bb][Ii][Tt];
		one_k =  [Oo][Nn][Ee];
		src_color_k =  [Ss][Rr][Cc]'_'?[Cc][Oo][Ll][Oo][Rr];
		inv_src_color_k =  [Ii][Nn][Vv]'_'?[Ss][Rr][Cc]'_'?[Cc][Oo][Ll][Oo][Rr];
		src_alpha_k =  [Ss][Rr][Cc]'_'?[Aa][Ll][Pp][Hh][Aa];
		inv_src_alpha_k =  [Ii][Nn][Vv]'_'?[Ss][Rr][Cc]'_'?[Aa][Ll][Pp][Hh][Aa];
		dest_alpha_k =  [Dd][Ee][Ss][Tt]'_'?[Aa][Ll][Pp][Hh][Aa];
		inv_dest_alpha_k =  [Ii][Nn][Vv]'_'?[Dd][Ee][Ss][Tt]'_'?[Aa][Ll][Pp][Hh][Aa];
		dest_color_k =  [Dd][Ee][Ss][Tt]'_'?[Cc][Oo][Ll][Oo][Rr];
		inv_dest_color_k =  [Ii][Nn][Vv]'_'?[Dd][Ee][Ss][Tt]'_'?[Cc][Oo][Ll][Oo][Rr];
		src_alpha_sat_k =  [Ss][Rr][Cc]'_'?[Aa][Ll][Pp][Hh][Aa]'_'?[Ss][Aa][Tt];
		blend_factor_k =  [Bb][Ll][Ee][Nn][Dd]'_'?[Ff][Aa][Cc][Tt][Oo][Rr];
		inv_blend_factor_k =  [Ii][Nn][Vv]'_'?[Bb][Ll][Ee][Nn][Dd]'_'?[Ff][Aa][Cc][Tt][Oo][Rr];
		src1_color_k =  [Ss][Rr][Cc][11]'_'?[Cc][Oo][Ll][Oo][Rr];
		inv_src1_color_k =  [Ii][Nn][Vv]'_'?[Ss][Rr][Cc][11]'_'?[Cc][Oo][Ll][Oo][Rr];
		src1_alpha_k =  [Ss][Rr][Cc][11]'_'?[Aa][Ll][Pp][Hh][Aa];
		inv_src1_alpha_k =  [Ii][Nn][Vv]'_'?[Ss][Rr][Cc][11]'_'?[Aa][Ll][Pp][Hh][Aa];
		clamp_k =  [Cc][Ll][Aa][Mm][Pp];
		wrap_k =  [Ww][Rr][Aa][Pp];
		mirror_k =  [Mm][Ii][Rr][Rr][Oo][Rr];
		border_k =  [Bb][Oo][Rr][Dd][Ee][Rr];
		mirror_once_k =  [Mm][Ii][Rr][Rr][Oo][Rr]'_'?[Oo][Nn][Cc][Ee];
		none_k =  [Nn][Oo][Nn][Ee];
		front_k =  [Ff][Rr][Oo][Nn][Tt];
		back_k =  [Bb][Aa][Cc][Kk];
		never_k =  [Nn][Ee][Vv][Ee][Rr];
		less_k =  [Ll][Ee][Ss][Ss];
		equal_k =  [Ee][Qq][Uu][Aa][Ll];
		less_equal_k =  [Ll][Ee][Ss][Ss]'_'?[Ee][Qq][Uu][Aa][Ll];
		greater_k =  [Gg][Rr][Ee][Aa][Tt][Ee][Rr];
		not_equal_k =  [Nn][Oo][Tt]'_'?[Ee][Qq][Uu][Aa][Ll];
		greater_equal_k =  [Gg][Rr][Ee][Aa][Tt][Ee][Rr]'_'?[Ee][Qq][Uu][Aa][Ll];
		always_k =  [Aa][Ll][Ww][Aa][Yy][Ss];
		keep_k =  [Kk][Ee][Ee][Pp];
		replace_k =  [Rr][Ee][Pp][Ll][Aa][Cc][Ee];
		incr_sat_k =  [Ii][Nn][Cc][Rr]'_'?[Ss][Aa][Tt];
		decr_sat_k =  [Dd][Ee][Cc][Rr]'_'?[Ss][Aa][Tt];
		invert_k =  [Ii][Nn][Vv][Ee][Rr][Tt];
		incr_k =  [Ii][Nn][Cc][Rr];
		decr_k =  [Dd][Ee][Cc][Rr];
		add_k =  [Aa][Dd][Dd];
		subtract_k =  [Ss][Uu][Bb][Tt][Rr][Aa][Cc][Tt];
		rev_subtract_k =  [Rr][Ee][Vv]'_'?[Ss][Uu][Bb][Tt][Rr][Aa][Cc][Tt];
		min_k =  [Mm][Ii][Nn];
		max_k =  [Mm][Aa][Xx];

		blend_op = [srcblend_k | destblend_k | srcblendalpha_k | destblendalpha_k];
		assign = "=";

	main :=  |*
		'[' space* [0-9] space* ']' => { tokens.push_back(Token(INDEX, atoi(ts+1))); };
		"{" => { tokens.push_back(Token(L_BRACE, 0)); };
		"}" => { tokens.push_back(Token(R_BRACE, 0)); };
		";" => { tokens.push_back(Token(SEMI_COLON, 0)); };
		assign => { tokens.push_back(Token(ASSIGN, 0)); };
		

		depthstencilstate_k => { tokens.push_back(Token(DEPTH_STENCIL_STATE_K,0));};
		rasterizerstate_k => { tokens.push_back(Token(RASTERIZER_STATE_K,0));};
		blendstate_k	=> { tokens.push_back(Token(BLEND_STATE_K,0));};
		samplerstate_k	=> { tokens.push_back(Token(SAMPLER_STATE_K,0));};

		false_k => { tokens.push_back(Token(BOOL_V, 0)); };
		true_k => { tokens.push_back(Token(BOOL_V, 1)); };
		
		all_k => { tokens.push_back(Token(DEPTH_WRITE_MASK_V, D3D11_DEPTH_WRITE_MASK_ALL)); };

		min_mag_mip_point_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_MIN_MAG_MIP_POINT)); };
		min_mag_point_mip_linear_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR)); };
		min_point_mag_linear_mip_point_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT)); };
		min_point_mag_mip_linear_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR)); };
		min_linear_mag_mip_point_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT)); };
		min_linear_mag_point_mip_linear_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR)); };
		min_mag_linear_mip_point_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT)); };
		anisotropic_k => { tokens.push_back(Token(FILTER_V, D3D11_FILTER_ANISOTROPIC)); };
		
		clamp_k => { tokens.push_back(Token(ADDRESS_V, D3D11_TEXTURE_ADDRESS_CLAMP)); };
		wrap_k => { tokens.push_back(Token(ADDRESS_V, D3D11_TEXTURE_ADDRESS_WRAP)); };
		mirror_k => { tokens.push_back(Token(ADDRESS_V, D3D11_TEXTURE_ADDRESS_MIRROR)); };
		border_k => { tokens.push_back(Token(ADDRESS_V, D3D11_TEXTURE_ADDRESS_BORDER)); };
		mirror_once_k => { tokens.push_back(Token(ADDRESS_V, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE)); };

		alphatocoverageenable_k => { tokens.push_back(Token(ALPHA_TO_COVERAGE_ENABLE_K,0));};
		blendenable_k => { tokens.push_back(Token(BLEND_ENABLE_K,0));};
		srcblend_k => { tokens.push_back(Token(SRC_BLEND_K,0));};
		destblend_k => { tokens.push_back(Token(DEST_BLEND_K,0));};
		blendop_k => { tokens.push_back(Token(BLEND_OP_K,0));};

		srcblendalpha_k => { tokens.push_back(Token(SRC_BLEND_ALPHA_K,0));};
		destblendalpha_k => { tokens.push_back(Token(DEST_BLEND_ALPHA_K,0));};
		blendopalpha_k => { tokens.push_back(Token(BLEND_OP_ALPHA_K,0));};
		rendertargetwritemask_k => { tokens.push_back(Token(RENDER_TARGET_WRITE_MASK_K,0));};

		blend_op space* assign space* zero_k => { tokens.push_back(Token(ASSIGN, 0)); tokens.push_back(Token(BLEND_V, D3D11_BLEND_ZERO)); };
		one_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_ONE)); };
		src_color_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_SRC_COLOR)); };
		inv_src_color_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_INV_SRC_COLOR)); };
		src_alpha_k =>  { tokens.push_back(Token(BLEND_V, D3D11_BLEND_SRC_ALPHA)); };
		inv_src_alpha_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_INV_SRC_ALPHA)); };
		dest_alpha_k =>  { tokens.push_back(Token(BLEND_V, D3D11_BLEND_DEST_ALPHA)); };
		inv_dest_alpha_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_INV_DEST_ALPHA)); };
		dest_color_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_DEST_COLOR)); };
		inv_dest_color_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_INV_DEST_COLOR)); };
		src_alpha_sat_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_SRC_ALPHA_SAT)); };
		blend_factor_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_BLEND_FACTOR)); };
		inv_blend_factor_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_INV_BLEND_FACTOR)); };
		src1_color_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_SRC1_COLOR)); };
		inv_src1_color_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_INV_SRC1_COLOR)); };
		src1_alpha_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_SRC1_ALPHA)); };
		inv_src1_alpha_k => { tokens.push_back(Token(BLEND_V, D3D11_BLEND_INV_SRC1_ALPHA)); };
		
		add_k => { tokens.push_back(Token(BLEND_OP_V, D3D11_BLEND_OP_ADD)); };
		subtract_k => { tokens.push_back(Token(BLEND_OP_V, D3D11_BLEND_OP_SUBTRACT)); };
		rev_subtract_k => { tokens.push_back(Token(BLEND_OP_V, D3D11_BLEND_OP_REV_SUBTRACT)); };
		min_k => { tokens.push_back(Token(BLEND_OP_V, D3D11_BLEND_OP_MIN)); };
		max_k => { tokens.push_back(Token(BLEND_OP_V, D3D11_BLEND_OP_MAX)); };

		fillmode_k => { tokens.push_back(Token(FILL_MODE_K, 0)); };
		solid_k => { tokens.push_back(Token(FILL_MODE_V, D3D11_FILL_SOLID)); };
		wireframe_k => { tokens.push_back(Token(FILL_MODE_V, D3D11_FILL_WIREFRAME)); };

		cullmode_k => { tokens.push_back(Token(CULL_MODE_K, 0)); };
		none_k => { tokens.push_back(Token(CULL_MODE_V, D3D11_CULL_NONE)); };
		front_k => { tokens.push_back(Token(CULL_MODE_V, D3D11_CULL_FRONT)); };
		back_k => { tokens.push_back(Token(CULL_MODE_V, D3D11_CULL_BACK)); };

		frontcounterclockwise_k => { tokens.push_back(Token(FRONT_COUNTER_CLOCKWISE_K, 0)); };
		depthbias_k => { tokens.push_back(Token(DEPTH_BIAS_K, 0)); };

		[0-9]+ => { tokens.push_back(Token(VALUE, atoi(ts))); };
		[0-9]+'.'[0-9]+ => { tokens.push_back(Token(FLOAT_VALUE, (float)atof(ts))); };
		'0'[xX]/[0-9][a-zA-z]/+ => { tokens.push_back(Token(VALUE, make_hex(ts, te))); };
		[a-zA-z][a-zA-Z_0-9]* => { tokens.push_back(Token(ID, (int)make_string(ts, te))); };


		space+ => { };
    *|;

    write init;
    write exec;
  }%%
 
  return 1;
}


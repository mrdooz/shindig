
#ifndef _MR_PARSER_HPP_
#define _MR_PARSER_HPP_
#include "D:\projects\shindig\shindig\parser\/state_parser_symbols.hpp"
class StateParser
{
public:
	StateParser(const Tokens& tokens) : _tokens(tokens), _idx(0) {}

	bool run()
	{
		return match_start();
	}
 BigState _states;
private:


bool match_filter_comparison_min_point_mag_linear_mip_point_t()
{
	return match(FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT);
}


bool match_rasterizer_depth_bias_t()
{
	return match(RASTERIZER_DEPTH_BIAS);
}


bool match_cull_mode_v_t()
{
	return match(CULL_MODE_V);
}


bool match_blend_src_blend_t()
{
	return match(BLEND_SRC_BLEND);
}


bool match_blend_blend_enable_t()
{
	return match(BLEND_BLEND_ENABLE);
}


bool match_pass_ab_sample_mask_t()
{
	return match(PASS_AB_SAMPLE_MASK);
}


bool match_filter_min_linear_mag_mip_point_t()
{
	return match(FILTER_MIN_LINEAR_MAG_MIP_POINT);
}


bool match_cull_mode_k_t()
{
	return match(CULL_MODE_K);
}


bool match_filter_min_linear_mag_point_mip_linear_t()
{
	return match(FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
}


bool match_depthstencil_depth_enable_t()
{
	return match(DEPTHSTENCIL_DEPTH_ENABLE);
}


bool match_filter_comparison_min_linear_mag_point_mip_linear_t()
{
	return match(FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
}


bool match_fill_wireframe_t()
{
	return match(FILL_WIREFRAME);
}


bool match_rasterizer_depth_bias_clamp_t()
{
	return match(RASTERIZER_DEPTH_BIAS_CLAMP);
}


bool match_filter_min_mag_mip_linear_t()
{
	return match(FILTER_MIN_MAG_MIP_LINEAR);
}


bool match_depthstencil_back_face_stencil_fail_t()
{
	return match(DEPTHSTENCIL_BACK_FACE_STENCIL_FAIL);
}


bool match_depthstencil_front_face_stencil_func_t()
{
	return match(DEPTHSTENCIL_FRONT_FACE_STENCIL_FUNC);
}


bool match_depth_write_mask_v_t()
{
	return match(DEPTH_WRITE_MASK_V);
}


bool match_rasterizer_fill_mode_t()
{
	return match(RASTERIZER_FILL_MODE);
}


bool match_render_target_view_t()
{
	return match(RENDER_TARGET_VIEW);
}


bool match_front_counter_clockwise_k_t()
{
	return match(FRONT_COUNTER_CLOCKWISE_K);
}


bool match_depthstencil_depth_write_mask_t()
{
	return match(DEPTHSTENCIL_DEPTH_WRITE_MASK);
}


bool match_sampler_address_w_t()
{
	return match(SAMPLER_ADDRESS_W);
}


bool match_l_brace_t()
{
	return match(L_BRACE);
}


bool match_assign_t()
{
	return match(ASSIGN);
}


bool match_pixel_shader_t()
{
	return match(PIXEL_SHADER);
}


bool match_pass_ds_stencil_ref_t()
{
	return match(PASS_DS_STENCIL_REF);
}


bool match_sampler_border_color_t()
{
	return match(SAMPLER_BORDER_COLOR);
}


bool match_filter_min_mag_point_mip_linear_t()
{
	return match(FILTER_MIN_MAG_POINT_MIP_LINEAR);
}


bool match_depthstencil_front_face_stencil_pass_t()
{
	return match(DEPTHSTENCIL_FRONT_FACE_STENCIL_PASS);
}


bool match_src_blend_k_t()
{
	return match(SRC_BLEND_K);
}


bool match_index_t()
{
	return match(INDEX);
}


bool match_depthstencil_back_face_stencil_func_t()
{
	return match(DEPTHSTENCIL_BACK_FACE_STENCIL_FUNC);
}


bool match_taddress_mirror_once_t()
{
	return match(TADDRESS_MIRROR_ONCE);
}


bool match_blend_render_target_write_mask_t()
{
	return match(BLEND_RENDER_TARGET_WRITE_MASK);
}


bool match_sampler_min_lod_t()
{
	return match(SAMPLER_MIN_LOD);
}


bool match_blend_state_k_t()
{
	return match(BLEND_STATE_K);
}


bool match_src_blend_alpha_k_t()
{
	return match(SRC_BLEND_ALPHA_K);
}


bool match_alpha_to_coverage_enable_k_t()
{
	return match(ALPHA_TO_COVERAGE_ENABLE_K);
}


bool match_address_v_t()
{
	return match(ADDRESS_V);
}


bool match_dest_blend_alpha_k_t()
{
	return match(DEST_BLEND_ALPHA_K);
}


bool match_rasterizer_multisample_enable_t()
{
	return match(RASTERIZER_MULTISAMPLE_ENABLE);
}


bool match_blend_dest_blend_alpha_t()
{
	return match(BLEND_DEST_BLEND_ALPHA);
}


bool match_comparison_min_linear_mag_mip_point_t()
{
	return match(COMPARISON_MIN_LINEAR_MAG_MIP_POINT);
}


bool match_depthstencil_front_face_stencil_fail_t()
{
	return match(DEPTHSTENCIL_FRONT_FACE_STENCIL_FAIL);
}


bool match_blend_dest_blend_t()
{
	return match(BLEND_DEST_BLEND);
}


bool match_rasterizer_front_counter_clockwise_t()
{
	return match(RASTERIZER_FRONT_COUNTER_CLOCKWISE);
}


bool match_render_target_write_mask_k_t()
{
	return match(RENDER_TARGET_WRITE_MASK_K);
}


bool match_filter_min_point_mag_linear_mip_point_t()
{
	return match(FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT);
}


bool match_filter_comparison_anisotropic_t()
{
	return match(FILTER_COMPARISON_ANISOTROPIC);
}


bool match_rasterizer_state_k_t()
{
	return match(RASTERIZER_STATE_K);
}


bool match_depthstencil_back_face_stencil_depth_fail_t()
{
	return match(DEPTHSTENCIL_BACK_FACE_STENCIL_DEPTH_FAIL);
}


bool match_filter_t()
{
	return match(FILTER);
}


bool match_blend_alpha_to_coverage_enable_t()
{
	return match(BLEND_ALPHA_TO_COVERAGE_ENABLE);
}


bool match_blend_op_alpha_k_t()
{
	return match(BLEND_OP_ALPHA_K);
}


bool match_vertex_shader_t()
{
	return match(VERTEX_SHADER);
}


bool match_sampler_address_u_t()
{
	return match(SAMPLER_ADDRESS_U);
}


bool match_depthstencil_back_face_stencil_pass_t()
{
	return match(DEPTHSTENCIL_BACK_FACE_STENCIL_PASS);
}


bool match_blend_src_blend_alpha_t()
{
	return match(BLEND_SRC_BLEND_ALPHA);
}


bool match_sampler_address_v_t()
{
	return match(SAMPLER_ADDRESS_V);
}


bool match_pass_generate_mips_t()
{
	return match(PASS_GENERATE_MIPS);
}


bool match_filter_min_point_mag_mip_linear_t()
{
	return match(FILTER_MIN_POINT_MAG_MIP_LINEAR);
}


bool match_depth_stencil_state_k_t()
{
	return match(DEPTH_STENCIL_STATE_K);
}


bool match_sampler_max_anisotropy_t()
{
	return match(SAMPLER_MAX_ANISOTROPY);
}


bool match_filter_text_1bit_t()
{
	return match(FILTER_TEXT_1BIT);
}


bool match_sampler_state_k_t()
{
	return match(SAMPLER_STATE_K);
}


bool match_semi_colon_t()
{
	return match(SEMI_COLON);
}


bool match_taddress_mirror_t()
{
	return match(TADDRESS_MIRROR);
}


bool match_fill_solid_t()
{
	return match(FILL_SOLID);
}


bool match_depthstencil_stencil_write_mask_t()
{
	return match(DEPTHSTENCIL_STENCIL_WRITE_MASK);
}


bool match_filter_anisotropic_t()
{
	return match(FILTER_ANISOTROPIC);
}


bool match_depthstencil_front_face_stencil_depth_fail_t()
{
	return match(DEPTHSTENCIL_FRONT_FACE_STENCIL_DEPTH_FAIL);
}


bool match_filter_comparison_min_mag_mip_point_t()
{
	return match(FILTER_COMPARISON_MIN_MAG_MIP_POINT);
}


bool match_filter_comparison_min_mag_point_mip_linear_t()
{
	return match(FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR);
}


bool match_depth_stencil_view_t()
{
	return match(DEPTH_STENCIL_VIEW);
}


bool match_filter_min_mag_linear_mip_point_t()
{
	return match(FILTER_MIN_MAG_LINEAR_MIP_POINT);
}


bool match_fill_mode_k_t()
{
	return match(FILL_MODE_K);
}


bool match_filter_comparison_min_mag_linear_mip_point_t()
{
	return match(FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT);
}


bool match_taddress_wrap_t()
{
	return match(TADDRESS_WRAP);
}


bool match_fill_mode_v_t()
{
	return match(FILL_MODE_V);
}


bool match_depthstencil_depth_enable_k_t()
{
	return match(DEPTHSTENCIL_DEPTH_ENABLE_K);
}


bool match_dest_blend_k_t()
{
	return match(DEST_BLEND_K);
}


bool match_rasterizer_depth_clip_enable_t()
{
	return match(RASTERIZER_DEPTH_CLIP_ENABLE);
}


bool match_taddress_clamp_t()
{
	return match(TADDRESS_CLAMP);
}


bool match_rasterizer_antialiased_line_enable_t()
{
	return match(RASTERIZER_ANTIALIASED_LINE_ENABLE);
}


bool match_sampler_mip_lodbias_t()
{
	return match(SAMPLER_MIP_LODBIAS);
}


bool match_blend_v_t()
{
	return match(BLEND_V);
}


bool match_blend_blend_op_t()
{
	return match(BLEND_BLEND_OP);
}


bool match_sampler_max_lod_t()
{
	return match(SAMPLER_MAX_LOD);
}


bool match_rasterizer_slope_scaled_depth_bias_t()
{
	return match(RASTERIZER_SLOPE_SCALED_DEPTH_BIAS);
}


bool match_blend_op_k_t()
{
	return match(BLEND_OP_K);
}


bool match_blend_op_v_t()
{
	return match(BLEND_OP_V);
}


bool match_float_value_t()
{
	return match(FLOAT_VALUE);
}


bool match_blend_enable_k_t()
{
	return match(BLEND_ENABLE_K);
}


bool match_filter_min_mag_mip_point_t()
{
	return match(FILTER_MIN_MAG_MIP_POINT);
}


bool match_blend_blend_op_alpha_t()
{
	return match(BLEND_BLEND_OP_ALPHA);
}


bool match_sampler_comparison_func_t()
{
	return match(SAMPLER_COMPARISON_FUNC);
}


bool match_filter_v_t()
{
	return match(FILTER_V);
}


bool match_pass_ab_blend_factor_t()
{
	return match(PASS_AB_BLEND_FACTOR);
}


bool match_rasterizer_cull_mode_t()
{
	return match(RASTERIZER_CULL_MODE);
}


bool match_rasterizer_scissor_enable_t()
{
	return match(RASTERIZER_SCISSOR_ENABLE);
}


bool match_bool_v_t()
{
	return match(BOOL_V);
}


bool match_taddress_border_t()
{
	return match(TADDRESS_BORDER);
}


bool match_geometry_shader_t()
{
	return match(GEOMETRY_SHADER);
}


bool match_sampler_filter_t()
{
	return match(SAMPLER_FILTER);
}


bool match_depthstencil_stencil_read_mask_t()
{
	return match(DEPTHSTENCIL_STENCIL_READ_MASK);
}


bool match_depthstencil_depth_func_t()
{
	return match(DEPTHSTENCIL_DEPTH_FUNC);
}


bool match_filter_comparison_min_mag_mip_linear_t()
{
	return match(FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
}


bool match_value_t()
{
	return match(VALUE);
}


bool match_depth_bias_k_t()
{
	return match(DEPTH_BIAS_K);
}


bool match_id_t()
{
	return match(ID);
}


bool match_depthstencil_stencil_enable_t()
{
	return match(DEPTHSTENCIL_STENCIL_ENABLE);
}


bool match_r_brace_t()
{
	return match(R_BRACE);
}


bool match_filter_comparison_min_point_mag_mip_linear_t()
{
	return match(FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR);
}


bool match_start_inner( )
{
    // start ::= state(_states)*
    
    
    
    
    if (!match_state(_states)) {
        
        return false;
    }

    
    return true;
}


bool match_start( )
{
    // start ::= state(_states)*
    
    int count = 0;
    while (match_start_inner()) {
        ++count;
    }
    if (count >= 0) {
        
        return true;
    } else {
        
        return false;
    }
}


bool match_state(BigState&  a)
{
    // state(a) : BigState& ::= [blend_states(a._blend_descs) | rasterizer_states(a._rasterizer_descs)]
    
    
    push_idx();
    if (match_blend_states(a._blend_descs) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_states(a._rasterizer_descs) ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_blend_states(BlendDescs&  a)
{
    // blend_states(a) : BlendDescs& ::= BLEND_STATE_K ID L_BRACE blend_state(a[$ID$._str]) R_BRACE SEMI_COLON
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[BLEND_STATE_K] = _idx;
    if (!match_blend_state_k_t()) {
        
        return false;
    }


    back_refs[ID] = _idx;
    if (!match_id_t()) {
        
        return false;
    }


    back_refs[L_BRACE] = _idx;
    if (!match_l_brace_t()) {
        
        return false;
    }


    
    if (!match_blend_state(a[_tokens[back_refs[ID]]._str])) {
        
        return false;
    }


    back_refs[R_BRACE] = _idx;
    if (!match_r_brace_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_blend_state_inner(D3D11_BLEND_DESC&  a)
{
    // blend_state(a) : D3D11_BLEND_DESC& ::= [alpha_to_coverage_state(a.AlphaToCoverageEnable) |blend_enable_state(&a.RenderTarget[0]) |blend_enable_state_def(&a.RenderTarget[0]) |src_blend_state(&a.RenderTarget[0]) |dest_blend_state(&a.RenderTarget[0]) |blend_op_state(&a.RenderTarget[0]) |src_blend_alpha_state(&a.RenderTarget[0]) |dest_blend_alpha_state(&a.RenderTarget[0]) |blend_op_alpha_state(&a.RenderTarget[0]) |render_target_write_mask_state(&a.RenderTarget[0]) ]*
    
    
    push_idx();
    if (match_alpha_to_coverage_state(a.AlphaToCoverageEnable) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_enable_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_enable_state_def(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_src_blend_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_dest_blend_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_op_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_src_blend_alpha_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_dest_blend_alpha_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_op_alpha_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_render_target_write_mask_state(&a.RenderTarget[0]) ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_blend_state(D3D11_BLEND_DESC&  a)
{
    // blend_state(a) : D3D11_BLEND_DESC& ::= [alpha_to_coverage_state(a.AlphaToCoverageEnable) |blend_enable_state(&a.RenderTarget[0]) |blend_enable_state_def(&a.RenderTarget[0]) |src_blend_state(&a.RenderTarget[0]) |dest_blend_state(&a.RenderTarget[0]) |blend_op_state(&a.RenderTarget[0]) |src_blend_alpha_state(&a.RenderTarget[0]) |dest_blend_alpha_state(&a.RenderTarget[0]) |blend_op_alpha_state(&a.RenderTarget[0]) |render_target_write_mask_state(&a.RenderTarget[0]) ]*
    
    int count = 0;
    while (match_blend_state_inner(a)) {
        ++count;
    }
    if (count >= 0) {
        
        return true;
    } else {
        
        return false;
    }
}


bool match_alpha_to_coverage_state(BOOL&  a)
{
    // alpha_to_coverage_state(a) : BOOL& ::= ALPHA_TO_COVERAGE_ENABLE_K ASSIGN BOOL_V SEMI_COLON@pass{ a = $BOOL_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[ALPHA_TO_COVERAGE_ENABLE_K] = _idx;
    if (!match_alpha_to_coverage_enable_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BOOL_V] = _idx;
    if (!match_bool_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a = _tokens[back_refs[BOOL_V]].extra;
    return true;
}


bool match_blend_enable_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // blend_enable_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= BLEND_ENABLE_K INDEX ASSIGN BOOL_V SEMI_COLON@pass{ a[$INDEX$.extra].BlendEnable = $BOOL_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[BLEND_ENABLE_K] = _idx;
    if (!match_blend_enable_k_t()) {
        
        return false;
    }


    back_refs[INDEX] = _idx;
    if (!match_index_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BOOL_V] = _idx;
    if (!match_bool_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[_tokens[back_refs[INDEX]].extra].BlendEnable = _tokens[back_refs[BOOL_V]].extra;
    return true;
}


bool match_blend_enable_state_def(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // blend_enable_state_def(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= BLEND_ENABLE_K ASSIGN BOOL_V SEMI_COLON@pass{ a[0].BlendEnable = $BOOL_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[BLEND_ENABLE_K] = _idx;
    if (!match_blend_enable_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BOOL_V] = _idx;
    if (!match_bool_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].BlendEnable = _tokens[back_refs[BOOL_V]].extra;
    return true;
}


bool match_src_blend_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // src_blend_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= SRC_BLEND_K ASSIGN BLEND_V SEMI_COLON@pass{ a[0].SrcBlend = (D3D11_BLEND)$BLEND_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[SRC_BLEND_K] = _idx;
    if (!match_src_blend_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BLEND_V] = _idx;
    if (!match_blend_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].SrcBlend = (D3D11_BLEND)_tokens[back_refs[BLEND_V]].extra;
    return true;
}


bool match_dest_blend_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // dest_blend_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= DEST_BLEND_K ASSIGN BLEND_V SEMI_COLON@pass{ a[0].DestBlend = (D3D11_BLEND)$BLEND_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[DEST_BLEND_K] = _idx;
    if (!match_dest_blend_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BLEND_V] = _idx;
    if (!match_blend_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].DestBlend = (D3D11_BLEND)_tokens[back_refs[BLEND_V]].extra;
    return true;
}


bool match_blend_op_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // blend_op_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= BLEND_OP_K ASSIGN BLEND_OP_V SEMI_COLON@pass{ a[0].BlendOp = (D3D11_BLEND_OP)$BLEND_OP_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[BLEND_OP_K] = _idx;
    if (!match_blend_op_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BLEND_OP_V] = _idx;
    if (!match_blend_op_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].BlendOp = (D3D11_BLEND_OP)_tokens[back_refs[BLEND_OP_V]].extra;
    return true;
}


bool match_src_blend_alpha_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // src_blend_alpha_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= SRC_BLEND_ALPHA_K ASSIGN BLEND_V SEMI_COLON@pass{ a[0].SrcBlendAlpha = (D3D11_BLEND)$BLEND_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[SRC_BLEND_ALPHA_K] = _idx;
    if (!match_src_blend_alpha_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BLEND_V] = _idx;
    if (!match_blend_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].SrcBlendAlpha = (D3D11_BLEND)_tokens[back_refs[BLEND_V]].extra;
    return true;
}


bool match_dest_blend_alpha_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // dest_blend_alpha_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= DEST_BLEND_ALPHA_K ASSIGN BLEND_V SEMI_COLON@pass{ a[0].DestBlendAlpha = (D3D11_BLEND)$BLEND_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[DEST_BLEND_ALPHA_K] = _idx;
    if (!match_dest_blend_alpha_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BLEND_V] = _idx;
    if (!match_blend_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].DestBlendAlpha = (D3D11_BLEND)_tokens[back_refs[BLEND_V]].extra;
    return true;
}


bool match_blend_op_alpha_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // blend_op_alpha_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= BLEND_OP_ALPHA_K ASSIGN BLEND_OP_V SEMI_COLON@pass{ a[0].BlendOpAlpha = (D3D11_BLEND_OP)$BLEND_OP_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[BLEND_OP_ALPHA_K] = _idx;
    if (!match_blend_op_alpha_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BLEND_OP_V] = _idx;
    if (!match_blend_op_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].BlendOpAlpha = (D3D11_BLEND_OP)_tokens[back_refs[BLEND_OP_V]].extra;
    return true;
}


bool match_render_target_write_mask_state(D3D11_RENDER_TARGET_BLEND_DESC*  a)
{
    // render_target_write_mask_state(a) : D3D11_RENDER_TARGET_BLEND_DESC* ::= RENDER_TARGET_WRITE_MASK_K INDEX ASSIGN VALUE SEMI_COLON@pass{ a[0].RenderTargetWriteMask = $VALUE$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[RENDER_TARGET_WRITE_MASK_K] = _idx;
    if (!match_render_target_write_mask_k_t()) {
        
        return false;
    }


    back_refs[INDEX] = _idx;
    if (!match_index_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[VALUE] = _idx;
    if (!match_value_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a[0].RenderTargetWriteMask = _tokens[back_refs[VALUE]].extra;
    return true;
}


bool match_rasterizer_states(RasterizerDescs&  a)
{
    // rasterizer_states(a) : RasterizerDescs& ::= RASTERIZER_STATE_K ID L_BRACE rasterizer_state(a[$ID$._str]) R_BRACE SEMI_COLON
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[RASTERIZER_STATE_K] = _idx;
    if (!match_rasterizer_state_k_t()) {
        
        return false;
    }


    back_refs[ID] = _idx;
    if (!match_id_t()) {
        
        return false;
    }


    back_refs[L_BRACE] = _idx;
    if (!match_l_brace_t()) {
        
        return false;
    }


    
    if (!match_rasterizer_state(a[_tokens[back_refs[ID]]._str])) {
        
        return false;
    }


    back_refs[R_BRACE] = _idx;
    if (!match_r_brace_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_rasterizer_state_inner(D3D11_RASTERIZER_DESC&  a)
{
    // rasterizer_state(a) : D3D11_RASTERIZER_DESC& ::= [fill_mode_state(a) |cull_mode_state(a)]*
    
    
    push_idx();
    if (match_fill_mode_state(a) ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_cull_mode_state(a) ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_rasterizer_state(D3D11_RASTERIZER_DESC&  a)
{
    // rasterizer_state(a) : D3D11_RASTERIZER_DESC& ::= [fill_mode_state(a) |cull_mode_state(a)]*
    
    int count = 0;
    while (match_rasterizer_state_inner(a)) {
        ++count;
    }
    if (count >= 0) {
        
        return true;
    } else {
        
        return false;
    }
}


bool match_fill_mode_state(D3D11_RASTERIZER_DESC&  a)
{
    // fill_mode_state(a) : D3D11_RASTERIZER_DESC& ::= FILL_MODE_K ASSIGN FILL_MODE_V SEMI_COLON@pass{ a.FillMode = (D3D11_FILL_MODE)$FILL_MODE_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[FILL_MODE_K] = _idx;
    if (!match_fill_mode_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[FILL_MODE_V] = _idx;
    if (!match_fill_mode_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a.FillMode = (D3D11_FILL_MODE)_tokens[back_refs[FILL_MODE_V]].extra;
    return true;
}


bool match_cull_mode_state(D3D11_RASTERIZER_DESC&  a)
{
    // cull_mode_state(a) : D3D11_RASTERIZER_DESC& ::= CULL_MODE_K ASSIGN CULL_MODE_V SEMI_COLON@pass{ a.CullMode = (D3D11_CULL_MODE)$CULL_MODE_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[CULL_MODE_K] = _idx;
    if (!match_cull_mode_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[CULL_MODE_V] = _idx;
    if (!match_cull_mode_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a.CullMode = (D3D11_CULL_MODE)_tokens[back_refs[CULL_MODE_V]].extra;
    return true;
}


bool match_front_counter_clockwise_state(D3D11_RASTERIZER_DESC&  a)
{
    // front_counter_clockwise_state(a) : D3D11_RASTERIZER_DESC& ::= FRONT_COUNTER_CLOCKWISE_K ASSIGN BOOL_V SEMI_COLON@pass{ a.FrontCounterClockwise = $BOOL_V$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[FRONT_COUNTER_CLOCKWISE_K] = _idx;
    if (!match_front_counter_clockwise_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[BOOL_V] = _idx;
    if (!match_bool_v_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a.FrontCounterClockwise = _tokens[back_refs[BOOL_V]].extra;
    return true;
}


bool match_depth_bias_state(D3D11_RASTERIZER_DESC&  a)
{
    // depth_bias_state(a) : D3D11_RASTERIZER_DESC& ::= DEPTH_BIAS_K ASSIGN VALUE SEMI_COLON@pass{ a.DepthBias = $VALUE$.extra; }
    std::map<TokenTag, int> back_refs;
    
    
    back_refs[DEPTH_BIAS_K] = _idx;
    if (!match_depth_bias_k_t()) {
        
        return false;
    }


    back_refs[ASSIGN] = _idx;
    if (!match_assign_t()) {
        
        return false;
    }


    back_refs[VALUE] = _idx;
    if (!match_value_t()) {
        
        return false;
    }


    back_refs[SEMI_COLON] = _idx;
    if (!match_semi_colon_t()) {
        
        return false;
    }

    a.DepthBias = _tokens[back_refs[VALUE]].extra;
    return true;
}


bool match_depth_stencil_states( )
{
    // depth_stencil_states ::= DEPTH_STENCIL_STATE_K ID L_BRACE depth_stencil_state R_BRACE SEMI_COLON
    
    
    
    
    if (!match_depth_stencil_state_k_t()) {
        
        return false;
    }


    
    if (!match_id_t()) {
        
        return false;
    }


    
    if (!match_l_brace_t()) {
        
        return false;
    }


    
    if (!match_depth_stencil_state()) {
        
        return false;
    }


    
    if (!match_r_brace_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_depth_stencil_state_inner( )
{
    // depth_stencil_state ::= [depth_enable_state | depth_write_mask_state]*
    
    
    push_idx();
    if (match_depth_enable_state() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depth_write_mask_state() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_depth_stencil_state( )
{
    // depth_stencil_state ::= [depth_enable_state | depth_write_mask_state]*
    
    int count = 0;
    while (match_depth_stencil_state_inner()) {
        ++count;
    }
    if (count >= 0) {
        
        return true;
    } else {
        
        return false;
    }
}


bool match_depth_enable_state( )
{
    // depth_enable_state ::= DEPTHSTENCIL_DEPTH_ENABLE_K ASSIGN BOOL_V SEMI_COLON
    
    
    
    
    if (!match_depthstencil_depth_enable_k_t()) {
        
        return false;
    }


    
    if (!match_assign_t()) {
        
        return false;
    }


    
    if (!match_bool_v_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_depth_write_mask_state( )
{
    // depth_write_mask_state ::= DEPTHSTENCIL_DEPTH_WRITE_MASK ASSIGN DEPTH_WRITE_MASK_V SEMI_COLON
    
    
    
    
    if (!match_depthstencil_depth_write_mask_t()) {
        
        return false;
    }


    
    if (!match_assign_t()) {
        
        return false;
    }


    
    if (!match_depth_write_mask_v_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_sampler_states( )
{
    // sampler_states ::= SAMPLER_STATE_K ID L_BRACE sampler_state R_BRACE SEMI_COLON
    
    
    
    
    if (!match_sampler_state_k_t()) {
        
        return false;
    }


    
    if (!match_id_t()) {
        
        return false;
    }


    
    if (!match_l_brace_t()) {
        
        return false;
    }


    
    if (!match_sampler_state()) {
        
        return false;
    }


    
    if (!match_r_brace_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_sampler_state_inner( )
{
    // sampler_state ::= [filter_state | addressu_state | addressv_state | addressw_state]*
    
    
    push_idx();
    if (match_filter_state() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_addressu_state() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_addressv_state() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_addressw_state() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_sampler_state( )
{
    // sampler_state ::= [filter_state | addressu_state | addressv_state | addressw_state]*
    
    int count = 0;
    while (match_sampler_state_inner()) {
        ++count;
    }
    if (count >= 0) {
        
        return true;
    } else {
        
        return false;
    }
}


bool match_ds_stencil_ref( )
{
    // ds_stencil_ref ::= PASS_DS_STENCIL_REF
    
    
    
    
    if (!match_pass_ds_stencil_ref_t()) {
        
        return false;
    }

    
    return true;
}


bool match_ab_blend_factor( )
{
    // ab_blend_factor ::= PASS_AB_BLEND_FACTOR
    
    
    
    
    if (!match_pass_ab_blend_factor_t()) {
        
        return false;
    }

    
    return true;
}


bool match_ab_sample_mask( )
{
    // ab_sample_mask ::= PASS_AB_SAMPLE_MASK
    
    
    
    
    if (!match_pass_ab_sample_mask_t()) {
        
        return false;
    }

    
    return true;
}


bool match_render_target_view( )
{
    // render_target_view ::= RENDER_TARGET_VIEW
    
    
    
    
    if (!match_render_target_view_t()) {
        
        return false;
    }

    
    return true;
}


bool match_depth_stencil_view( )
{
    // depth_stencil_view ::= DEPTH_STENCIL_VIEW
    
    
    
    
    if (!match_depth_stencil_view_t()) {
        
        return false;
    }

    
    return true;
}


bool match_generate_mips( )
{
    // generate_mips ::= PASS_GENERATE_MIPS
    
    
    
    
    if (!match_pass_generate_mips_t()) {
        
        return false;
    }

    
    return true;
}


bool match_vertex_shader( )
{
    // vertex_shader ::= VERTEX_SHADER
    
    
    
    
    if (!match_vertex_shader_t()) {
        
        return false;
    }

    
    return true;
}


bool match_pixel_shader( )
{
    // pixel_shader ::= PIXEL_SHADER
    
    
    
    
    if (!match_pixel_shader_t()) {
        
        return false;
    }

    
    return true;
}


bool match_geometry_shader( )
{
    // geometry_shader ::= GEOMETRY_SHADER
    
    
    
    
    if (!match_geometry_shader_t()) {
        
        return false;
    }

    
    return true;
}


bool match_cull_mode( )
{
    // cull_mode ::= RASTERIZER_CULL_MODE
    
    
    
    
    if (!match_rasterizer_cull_mode_t()) {
        
        return false;
    }

    
    return true;
}


bool match_filter_state( )
{
    // filter_state ::= FILTER ASSIGN FILTER_V SEMI_COLON
    
    
    
    
    if (!match_filter_t()) {
        
        return false;
    }


    
    if (!match_assign_t()) {
        
        return false;
    }


    
    if (!match_filter_v_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_addressu_state( )
{
    // addressu_state ::= SAMPLER_ADDRESS_U ASSIGN ADDRESS_V SEMI_COLON
    
    
    
    
    if (!match_sampler_address_u_t()) {
        
        return false;
    }


    
    if (!match_assign_t()) {
        
        return false;
    }


    
    if (!match_address_v_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_addressv_state( )
{
    // addressv_state ::= SAMPLER_ADDRESS_V ASSIGN ADDRESS_V SEMI_COLON
    
    
    
    
    if (!match_sampler_address_v_t()) {
        
        return false;
    }


    
    if (!match_assign_t()) {
        
        return false;
    }


    
    if (!match_address_v_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_addressw_state( )
{
    // addressw_state ::= SAMPLER_ADDRESS_W ASSIGN ADDRESS_V SEMI_COLON
    
    
    
    
    if (!match_sampler_address_w_t()) {
        
        return false;
    }


    
    if (!match_assign_t()) {
        
        return false;
    }


    
    if (!match_address_v_t()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_fill_mode( )
{
    // fill_mode ::= RASTERIZER_FILL_MODE ASSIGN fill_mode_values SEMI_COLON
    
    
    
    
    if (!match_rasterizer_fill_mode_t()) {
        
        return false;
    }


    
    if (!match_assign_t()) {
        
        return false;
    }


    
    if (!match_fill_mode_values()) {
        
        return false;
    }


    
    if (!match_semi_colon_t()) {
        
        return false;
    }

    
    return true;
}


bool match_fill_mode_values( )
{
    // fill_mode_values ::= [FILL_WIREFRAME | FILL_SOLID]
    
    
    push_idx();
    if (match_fill_wireframe_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_fill_solid_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_tmp( )
{
    // tmp ::= [ BLEND_ALPHA_TO_COVERAGE_ENABLE | BLEND_BLEND_ENABLE | VALUE | FLOAT_VALUE]
    
    
    push_idx();
    if (match_blend_alpha_to_coverage_enable_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_blend_enable_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_value_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_float_value_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_filter_state_values( )
{
    // filter_state_values ::= [FILTER_MIN_MAG_MIP_POINT | FILTER_MIN_MAG_POINT_MIP_LINEAR | FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT |FILTER_MIN_POINT_MAG_MIP_LINEAR | FILTER_MIN_LINEAR_MAG_MIP_POINT | FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR |FILTER_MIN_MAG_LINEAR_MIP_POINT | FILTER_MIN_MAG_MIP_LINEAR | FILTER_ANISOTROPIC |FILTER_COMPARISON_MIN_MAG_MIP_POINT | FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR | FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT |FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR | COMPARISON_MIN_LINEAR_MAG_MIP_POINT | FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR |FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT | FILTER_COMPARISON_MIN_MAG_MIP_LINEAR | FILTER_COMPARISON_ANISOTROPIC |FILTER_TEXT_1BIT]
    
    
    push_idx();
    if (match_filter_min_mag_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_min_mag_point_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_min_point_mag_linear_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_min_point_mag_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_min_linear_mag_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_min_linear_mag_point_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_min_mag_linear_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_min_mag_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_anisotropic_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_min_mag_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_min_mag_point_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_min_point_mag_linear_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_min_point_mag_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_comparison_min_linear_mag_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_min_linear_mag_point_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_min_mag_linear_mip_point_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_min_mag_mip_linear_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_comparison_anisotropic_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_filter_text_1bit_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_taddress_values( )
{
    // taddress_values ::= [TADDRESS_CLAMP | TADDRESS_WRAP | TADDRESS_MIRROR | TADDRESS_BORDER | TADDRESS_MIRROR_ONCE]
    
    
    push_idx();
    if (match_taddress_clamp_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_taddress_wrap_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_taddress_mirror_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_taddress_border_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_taddress_mirror_once_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_blend_state_values( )
{
    // blend_state_values ::= [BLEND_SRC_BLEND | BLEND_DEST_BLEND | BLEND_BLEND_OP | BLEND_SRC_BLEND_ALPHA |BLEND_DEST_BLEND_ALPHA | BLEND_BLEND_OP_ALPHA | BLEND_RENDER_TARGET_WRITE_MASK]
    
    
    push_idx();
    if (match_blend_src_blend_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_dest_blend_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_blend_op_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_src_blend_alpha_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_dest_blend_alpha_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_blend_op_alpha_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_blend_render_target_write_mask_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_rasterizer_state_values( )
{
    // rasterizer_state_values ::= [RASTERIZER_FRONT_COUNTER_CLOCKWISE | RASTERIZER_DEPTH_BIAS | RASTERIZER_DEPTH_BIAS_CLAMP |RASTERIZER_SLOPE_SCALED_DEPTH_BIAS | RASTERIZER_DEPTH_CLIP_ENABLE | RASTERIZER_SCISSOR_ENABLE |RASTERIZER_MULTISAMPLE_ENABLE | RASTERIZER_ANTIALIASED_LINE_ENABLE]
    
    
    push_idx();
    if (match_rasterizer_front_counter_clockwise_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_depth_bias_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_depth_bias_clamp_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_slope_scaled_depth_bias_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_depth_clip_enable_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_scissor_enable_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_multisample_enable_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_rasterizer_antialiased_line_enable_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_sampler_state_values( )
{
    // sampler_state_values ::= [SAMPLER_FILTER | SAMPLER_ADDRESS_U | SAMPLER_ADDRESS_V | SAMPLER_ADDRESS_W |SAMPLER_MIP_LODBIAS | SAMPLER_MAX_ANISOTROPY | SAMPLER_COMPARISON_FUNC | SAMPLER_BORDER_COLOR |SAMPLER_MIN_LOD | SAMPLER_MAX_LOD]
    
    
    push_idx();
    if (match_sampler_filter_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_address_u_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_address_v_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_address_w_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_mip_lodbias_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_max_anisotropy_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_comparison_func_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_border_color_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_min_lod_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_sampler_max_lod_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


bool match_dss_values( )
{
    // dss_values ::= [ DEPTHSTENCIL_DEPTH_ENABLE | DEPTHSTENCIL_DEPTH_WRITE_MASK | DEPTHSTENCIL_DEPTH_FUNC |DEPTHSTENCIL_STENCIL_ENABLE | DEPTHSTENCIL_STENCIL_READ_MASK | DEPTHSTENCIL_STENCIL_WRITE_MASK |DEPTHSTENCIL_FRONT_FACE_STENCIL_FAIL | DEPTHSTENCIL_FRONT_FACE_STENCIL_DEPTH_FAIL |DEPTHSTENCIL_FRONT_FACE_STENCIL_PASS | DEPTHSTENCIL_FRONT_FACE_STENCIL_FUNC |DEPTHSTENCIL_BACK_FACE_STENCIL_FAIL | DEPTHSTENCIL_BACK_FACE_STENCIL_DEPTH_FAIL |DEPTHSTENCIL_BACK_FACE_STENCIL_PASS | DEPTHSTENCIL_BACK_FACE_STENCIL_FUNC]
    
    
    push_idx();
    if (match_depthstencil_depth_enable_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_depth_write_mask_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_depth_func_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_stencil_enable_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_stencil_read_mask_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_stencil_write_mask_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_front_face_stencil_fail_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_front_face_stencil_depth_fail_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_front_face_stencil_pass_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_front_face_stencil_func_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_back_face_stencil_fail_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_back_face_stencil_depth_fail_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_back_face_stencil_pass_t() ) {
        
        return true;
    }
    pop_idx();


    push_idx();
    if (match_depthstencil_back_face_stencil_func_t() ) {
        
        return true;
    }
    pop_idx();

    
    return false;
}


	void push_idx()
	{
		_idx_stack.push(_idx);
	}

	void pop_idx()
	{
		_idx = _idx_stack.top();		
		_idx_stack.pop();
	}

	bool match(const TokenTag cur)
	{
		if (cur == lookahead()) {
			_idx++;
			return true;
		}
	return false;
	}
	
	TokenTag lookahead()
	{
		if (_idx >= (int)_tokens.size()) {
			return INVALID;
		}
		return _tokens[_idx].tag;
	}

	typedef std::stack<int> IdxStack;
	IdxStack _idx_stack;
	int _idx;
	Tokens _tokens;

};
#endif

export interface PrivClass {
	readonly type: 'Class'
	readonly class_name: string
	readonly options: { 
		[key: string]: { 
			name: string 
			help: string
			option_type: string
			flags: {
				ENCODING_PARAM: boolean
				DECODING_PARAM: boolean
				AUDIO_PARAM: boolean
				VIDEO_PARAM: boolean
				SUBTITLE_PARAM: boolean
				EXPORT: boolean
				READONLY: boolean
				BSF_PARAM: boolean
				FILTERING_PARAM: boolean
				DEPRECATED: boolean
			}
			unit?: string
			const?: Array<string>
		}
	}
}
